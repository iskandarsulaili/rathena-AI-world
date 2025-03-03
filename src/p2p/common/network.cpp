#include "network.hpp"
#include <cstring>
#include <iostream>
#include <arpa/inet.h>

namespace rathena {
namespace p2p {

// NetworkPacket implementation
NetworkPacket::NetworkPacket(PacketType type)
    : type_(type)
    , read_pos_(0)
    , sequence_(0)
    , flags_(0) {
}

NetworkPacket::NetworkPacket(const uint8_t* data, size_t length)
    : read_pos_(0)
    , sequence_(0)
    , flags_(0) {
    
    // Read packet header
    if (length < sizeof(PacketHeader)) {
        throw std::runtime_error("Invalid packet size");
    }
    
    const PacketHeader* header = reinterpret_cast<const PacketHeader*>(data);
    type_ = static_cast<PacketType>(ntohs(header->type));
    sequence_ = ntohl(header->sequence);
    flags_ = header->flags;
    
    // Copy packet data
    data_.assign(data + sizeof(PacketHeader), data + length);
}

// Write methods
void NetworkPacket::write_uint8(uint8_t value) {
    data_.push_back(value);
}

void NetworkPacket::write_uint16(uint16_t value) {
    value = htons(value);
    data_.insert(data_.end(), reinterpret_cast<uint8_t*>(&value),
                 reinterpret_cast<uint8_t*>(&value) + sizeof(value));
}

void NetworkPacket::write_uint32(uint32_t value) {
    value = htonl(value);
    data_.insert(data_.end(), reinterpret_cast<uint8_t*>(&value),
                 reinterpret_cast<uint8_t*>(&value) + sizeof(value));
}

void NetworkPacket::write_uint64(uint64_t value) {
    value = htobe64(value);
    data_.insert(data_.end(), reinterpret_cast<uint8_t*>(&value),
                 reinterpret_cast<uint8_t*>(&value) + sizeof(value));
}

void NetworkPacket::write_float(float value) {
    static_assert(sizeof(float) == sizeof(uint32_t), "Float size mismatch");
    uint32_t raw;
    std::memcpy(&raw, &value, sizeof(float));
    write_uint32(raw);
}

void NetworkPacket::write_string(const std::string& value) {
    write_uint16(static_cast<uint16_t>(value.length()));
    data_.insert(data_.end(), value.begin(), value.end());
}

void NetworkPacket::write_buffer(const std::vector<uint8_t>& buffer) {
    write_uint32(static_cast<uint32_t>(buffer.size()));
    data_.insert(data_.end(), buffer.begin(), buffer.end());
}

// Read methods
uint8_t NetworkPacket::read_uint8() const {
    if (read_pos_ + sizeof(uint8_t) > data_.size()) {
        throw std::runtime_error("Packet read overflow");
    }
    return data_[read_pos_++];
}

uint16_t NetworkPacket::read_uint16() const {
    if (read_pos_ + sizeof(uint16_t) > data_.size()) {
        throw std::runtime_error("Packet read overflow");
    }
    uint16_t value;
    std::memcpy(&value, &data_[read_pos_], sizeof(value));
    read_pos_ += sizeof(value);
    return ntohs(value);
}

uint32_t NetworkPacket::read_uint32() const {
    if (read_pos_ + sizeof(uint32_t) > data_.size()) {
        throw std::runtime_error("Packet read overflow");
    }
    uint32_t value;
    std::memcpy(&value, &data_[read_pos_], sizeof(value));
    read_pos_ += sizeof(value);
    return ntohl(value);
}

float NetworkPacket::read_float() const {
    uint32_t raw = read_uint32();
    float value;
    std::memcpy(&value, &raw, sizeof(float));
    return value;
}

std::string NetworkPacket::read_string() const {
    uint16_t length = read_uint16();
    if (read_pos_ + length > data_.size()) {
        throw std::runtime_error("Packet read overflow");
    }
    std::string value(reinterpret_cast<const char*>(&data_[read_pos_]), length);
    read_pos_ += length;
    return value;
}

std::vector<uint8_t> NetworkPacket::read_buffer(size_t length) const {
    if (read_pos_ + length > data_.size()) {
        throw std::runtime_error("Packet read overflow");
    }
    std::vector<uint8_t> buffer(data_.begin() + read_pos_,
                               data_.begin() + read_pos_ + length);
    read_pos_ += length;
    return buffer;
}

void NetworkPacket::set_encrypted(bool value) {
    if (value) {
        flags_ |= FLAG_ENCRYPTED;
    } else {
        flags_ &= ~FLAG_ENCRYPTED;
    }
}

void NetworkPacket::set_compressed(bool value) {
    if (value) {
        flags_ |= FLAG_COMPRESSED;
    } else {
        flags_ &= ~FLAG_COMPRESSED;
    }
}

// NetworkSession implementation
NetworkSession::NetworkSession(const NetworkAddress& remote_address)
    : remote_address_(remote_address)
    , connected_(false)
    , encrypted_(false)
    , latency_(0)
    , last_receive_(current_time()) {
}

NetworkSession::~NetworkSession() {
    connected_ = false;
}

void NetworkSession::queue_packet(const NetworkPacket& packet) {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    outgoing_queue_.push(packet);
}

void NetworkSession::process_incoming(const uint8_t* data, size_t length) {
    try {
        NetworkPacket packet(data, length);
        
        // Update session state
        last_receive_ = current_time();
        
        // Process packet based on type
        auto session = shared_from_this();
        packet.set_session(session);
        
        // Queue for processing
        if (auto manager = manager_.lock()) {
            manager->handle_packet(packet);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error processing incoming packet: " << e.what() << std::endl;
    }
}

// NetworkManager implementation
NetworkManager::NetworkManager()
    : running_(false)
    , compression_threshold_(1024)
    , keep_alive_interval_(30000)
    , timeout_(60000)
    , active_sessions_(0)
    , packets_per_second_(0)
    , last_metrics_update_(current_time()) {
}

NetworkManager::~NetworkManager() {
    if (running_) {
        stop_server();
    }
}

bool NetworkManager::start_server(uint16_t port) {
    if (running_) {
        return false;
    }

    try {
        // Initialize socket and bind
        // TODO: Implement actual socket initialization
        
        running_ = true;
        network_thread_ = std::thread(&NetworkManager::network_loop, this);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to start server: " << e.what() << std::endl;
        return false;
    }
}

void NetworkManager::stop_server() {
    if (!running_) {
        return;
    }

    running_ = false;
    if (network_thread_.joinable()) {
        network_thread_.join();
    }

    // Clean up sessions
    std::lock_guard<std::mutex> lock(sessions_mutex_);
    sessions_.clear();
    active_sessions_ = 0;
}

void NetworkManager::register_handler(PacketType type, PacketHandler handler) {
    handlers_[type] = std::move(handler);
}

void NetworkManager::send_packet(const std::shared_ptr<NetworkSession>& session,
                               const NetworkPacket& packet) {
    if (!session || !session->is_connected()) {
        return;
    }

    try {
        // Prepare packet for sending
        std::vector<uint8_t> buffer;
        PacketHeader header{
            .length = static_cast<uint16_t>(sizeof(PacketHeader) + packet.get_length()),
            .type = static_cast<uint16_t>(packet.get_type()),
            .sequence = packet.get_sequence(),
            .timestamp = static_cast<uint32_t>(current_time()),
            .flags = packet.flags_
        };

        // Convert header to network byte order
        header.length = htons(header.length);
        header.type = htons(header.type);
        header.sequence = htonl(header.sequence);
        header.timestamp = htonl(header.timestamp);

        // Build final packet
        buffer.resize(header.length);
        std::memcpy(buffer.data(), &header, sizeof(header));
        std::memcpy(buffer.data() + sizeof(header), packet.get_data(), packet.get_length());

        // Queue for sending
        session->queue_packet(NetworkPacket(buffer.data(), buffer.size()));

    } catch (const std::exception& e) {
        std::cerr << "Error sending packet: " << e.what() << std::endl;
    }
}

uint32_t NetworkManager::get_average_latency() const {
    uint32_t total = 0;
    uint32_t count = 0;

    std::lock_guard<std::mutex> lock(sessions_mutex_);
    for (const auto& session : sessions_) {
        if (session->is_connected()) {
            total += session->get_latency();
            count++;
        }
    }

    return count > 0 ? total / count : 0;
}

void NetworkManager::network_loop() {
    while (running_) {
        process_incoming();
        process_outgoing();
        check_timeouts();
        send_keep_alive();
        
        // Update metrics
        auto now = current_time();
        if (now - last_metrics_update_ >= 1000) {
            packets_per_second_ = 0;
            last_metrics_update_ = now;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void NetworkManager::check_timeouts() {
    auto now = current_time();
    std::lock_guard<std::mutex> lock(sessions_mutex_);
    
    for (auto it = sessions_.begin(); it != sessions_.end();) {
        auto session = *it;
        if (session->is_connected() &&
            now - session->get_last_receive() > timeout_) {
            // Session timed out
            session->connected_ = false;
            it = sessions_.erase(it);
            active_sessions_--;
        } else {
            ++it;
        }
    }
}

} // namespace p2p
} // namespace rathena