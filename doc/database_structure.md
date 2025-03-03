# rAthena Database Structure Documentation

## Account & Login System

### 1. `login` - Player Account Information
- Purpose: Stores all account-related information and authentication data
- Fields:
  - `account_id` int(11) - Unique account identifier (Primary Key, Auto Increment)
  - `userid` varchar(23) - Account username
  - `user_pass` varchar(32) - Account password hash
  - `sex` enum('M','F','S') - Account gender (M=Male, F=Female, S=Server)
  - `email` varchar(39) - Account email address
  - `group_id` tinyint(3) - Account group/permission level
  - `state` int(11) - Account state flags
  - `unban_time` int(11) - Time when account ban expires
  - `expiration_time` int(11) - Account subscription expiration
  - `logincount` mediumint(9) - Number of successful logins
  - `lastlogin` datetime - Last login timestamp
  - `last_ip` varchar(100) - Last login IP address
  - `birthdate` DATE - Account holder's birth date
  - `character_slots` tinyint(3) - Available character slots
  - `pincode` varchar(4) - Security PIN code
  - `pincode_change` int(11) - PIN code last change time
  - `vip_time` int(11) - VIP status expiration time
  - `web_auth_token` varchar(17) - Web authentication token
  - `web_auth_token_enabled` tinyint(2) - Web auth status

### 2. `ipbanlist` - IP Ban System
- Purpose: Manages IP address bans and their durations
- Fields:
  - `list` varchar(15) - Banned IP address (Primary Key)
  - `btime` datetime - Ban start time
  - `rtime` datetime - Ban release time
  - `reason` varchar(255) - Ban reason

## Character System

### 1. `char` - Character Data
- Purpose: Stores core character information and statistics
- Fields:
  - `char_id` int(11) - Unique character identifier (Primary Key)
  - `account_id` int(11) - Associated account ID
  - `char_num` tinyint(1) - Character slot number
  - `name` varchar(30) - Character name
  - `class` smallint(6) - Character job class
  - `base_level` smallint(6) - Base experience level
  - `job_level` smallint(6) - Job experience level
  - `base_exp` bigint(20) - Current base experience
  - `job_exp` bigint(20) - Current job experience
  - `zeny` int(11) - Character's money
  - Stats:
    - `str`, `agi`, `vit`, `int`, `dex`, `luk` - Base stats
    - `pow`, `sta`, `wis`, `spl`, `con`, `crt` - Extended stats
  - HP/SP:
    - `max_hp`, `hp` - Maximum and current HP
    - `max_sp`, `sp` - Maximum and current SP
    - `max_ap`, `ap` - Maximum and current AP
  - Points:
    - `status_point` - Available stat points
    - `skill_point` - Available skill points
    - `trait_point` - Available trait points
  - Appearance:
    - `hair`, `hair_color` - Hair style and color
    - `clothes_color` - Clothing color
    - `body` - Body type
    - `weapon`, `shield` - Equipped items
    - `head_top`, `head_mid`, `head_bottom` - Head equipment
    - `robe` - Robe equipment
  - Location:
    - `last_map`, `last_x`, `last_y` - Last position
    - `save_map`, `save_x`, `save_y` - Save point
  - Social:
    - `party_id` - Party membership
    - `guild_id` - Guild membership
    - `pet_id` - Pet ownership
    - `homun_id` - Homunculus ownership
    - `elemental_id` - Elemental ownership
    - `clan_id` - Clan membership

### 2. `hotkey` - Hotkey Configuration
- Purpose: Stores character hotkey bindings
- Fields:
  - `char_id` INT(11) - Character ID (Primary Key part 1)
  - `hotkey` TINYINT(2) - Hotkey slot number (Primary Key part 2)
  - `type` TINYINT(1) - Type of hotkey
  - `itemskill_id` INT(11) - Item or skill ID
  - `skill_lvl` TINYINT(4) - Skill level for skill hotkeys

### 3. `sc_data` - Status Effects
- Purpose: Tracks active status effects/buffs
- Fields:
  - `account_id` int(11) - Account ID
  - `char_id` int(11) - Character ID (Primary Key part 1)
  - `type` smallint(11) - Effect type (Primary Key part 2)
  - `tick` bigint(20) - Duration tick
  - `val1` to `val4` int(11) - Effect parameters

## Inventory Systems

### 1. `inventory` - Character Inventory
- Purpose: Manages character item storage
- Fields:
  - `id` int(11) - Unique item instance ID (Primary Key)
  - `char_id` int(11) - Owner character ID
  - `nameid` int(10) - Item ID
  - `amount` int(11) - Stack quantity
  - `equip` int(11) - Equipment slot
  - `identify` smallint(6) - Identification status
  - `refine` tinyint(3) - Refinement level
  - `attribute` tinyint(4) - Item attributes
  - Cards:
    - `card0` to `card3` int(10) - Card slots
  - Options:
    - `option_id0` to `option_id4` smallint(5) - Random option IDs
    - `option_val0` to `option_val4` smallint(5) - Random option values
    - `option_parm0` to `option_parm4` tinyint(3) - Random option parameters
  - `expire_time` int(11) - Item expiration time
  - `favorite` tinyint(3) - Favorite flag
  - `bound` tinyint(3) - Binding type
  - `unique_id` bigint(20) - Unique identifier
  - `equip_switch` int(11) - Equipment switch flag
  - `enchantgrade` tinyint - Enchant grade level

### 2. `storage` - Account Storage
- Purpose: Account-wide item storage system
- Fields: (Similar structure to inventory)
  - `id` int(11) - Unique storage entry ID
  - `account_id` int(11) - Owner account ID
  - (Other fields match inventory structure)

### 3. `cart_inventory` - Cart Storage
- Purpose: Character cart item storage
- Fields: (Similar structure to inventory)
  - `id` int(11) - Unique cart item ID
  - `char_id` int(11) - Owner character ID
  - (Other fields match inventory structure)

### 4. `guild_storage` - Guild Storage
- Purpose: Guild-wide item storage system
- Fields: (Similar structure to inventory)
  - `id` int(11) - Unique guild storage entry ID
  - `guild_id` int(11) - Owner guild ID
  - (Other fields match inventory structure)

## Guild System

### 1. `guild` - Guild Information
- Purpose: Core guild data storage
- Fields:
  - `guild_id` int(11) - Unique guild identifier (Primary Key)
  - `name` varchar(24) - Guild name
  - `char_id` int(11) - Guild leader character ID
  - `master` varchar(24) - Guild leader name
  - `guild_lv` tinyint(6) - Guild level
  - `connect_member` tinyint(6) - Online member count
  - `max_member` tinyint(6) - Maximum members allowed
  - `average_lv` smallint(6) - Average member level
  - `exp` bigint(20) - Guild experience points
  - `next_exp` bigint(20) - Experience for next level
  - `skill_point` tinyint(11) - Available guild skill points
  - `mes1` varchar(60) - Guild notice message 1
  - `mes2` varchar(120) - Guild notice message 2
  - `emblem_len` int(11) - Guild emblem data length
  - `emblem_id` int(11) - Guild emblem identifier
  - `emblem_data` blob - Guild emblem binary data

### 2. `guild_alliance` - Guild Relations
- Purpose: Tracks guild alliances and oppositions
- Fields:
  - `guild_id` int(11) - Primary guild ID (Primary Key part 1)
  - `opposition` int(11) - Alliance type flag
  - `alliance_id` int(11) - Target guild ID (Primary Key part 2)
  - `name` varchar(24) - Alliance guild name

### 3. `guild_castle` - Castle Control
- Purpose: Manages guild castle ownership and economy
- Fields:
  - `castle_id` int(11) - Castle identifier (Primary Key)
  - `guild_id` int(11) - Controlling guild ID
  - `economy` int(11) - Castle economy points
  - `defense` int(11) - Castle defense rating
  - `triggerE` int(11) - Economy trigger
  - `triggerD` int(11) - Defense trigger
  - `nextTime` int(11) - Next update time
  - `payTime` int(11) - Next payment time
  - `createTime` int(11) - Castle creation time
  - `visibleC` to `visibleG7` int(11) - Visibility flags

### 4. `guild_member` - Guild Members
- Purpose: Tracks guild membership
- Fields:
  - `guild_id` int(11) - Guild ID (Primary Key part 1)
  - `char_id` int(11) - Member character ID (Primary Key part 2)
  - `exp` bigint(20) - Contribution experience
  - `position` tinyint(6) - Position in guild

### 5. `guild_position` - Guild Positions
- Purpose: Defines guild ranks and permissions
- Fields:
  - `guild_id` int(9) - Guild ID (Primary Key part 1)
  - `position` tinyint(6) - Position ID (Primary Key part 2)
  - `name` varchar(24) - Position title
  - `mode` smallint(11) - Permission bitmask
  - `exp_mode` tinyint(11) - Experience sharing mode

### 6. `guild_skill` - Guild Skills
- Purpose: Tracks guild skill levels
- Fields:
  - `guild_id` int(11) - Guild ID (Primary Key part 1)
  - `id` smallint(11) - Skill ID (Primary Key part 2)
  - `lv` tinyint(11) - Skill level

## Party System

### 1. `party` - Party Groups
- Purpose: Manages party groups
- Fields:
  - `party_id` int(11) - Unique party ID (Primary Key)
  - `name` varchar(24) - Party name
  - `exp` tinyint(11) - Experience sharing setting
  - `item` tinyint(11) - Item sharing setting
  - `leader_id` int(11) - Party leader account ID
  - `leader_char` int(11) - Party leader character ID

### 2. `party_bookings` - Party Recruitment
- Purpose: Party recruitment listings
- Fields:
  - `world_name` varchar(32) - Game world name (Primary Key part 1)
  - `account_id` int(11) - Recruiter account ID (Primary Key part 2)
  - `char_id` int(11) - Recruiter character ID (Primary Key part 3)
  - `char_name` varchar(23) - Recruiter character name
  - `purpose` smallint(5) - Party purpose/type
  - Role requirements:
    - `assist` tinyint(3) - Support role needed
    - `damagedealer` tinyint(3) - DPS role needed
    - `healer` tinyint(3) - Healer role needed
    - `tanker` tinyint(3) - Tank role needed
  - `minimum_level` smallint(5) - Minimum level requirement
  - `maximum_level` smallint(5) - Maximum level requirement
  - `comment` varchar(255) - Recruitment message

## Pet System

### 1. `pet` - Pet Information
- Purpose: Manages pet data and status
- Fields:
  - `pet_id` int(11) - Unique pet ID (Primary Key)
  - `class` mediumint(9) - Pet monster class
  - `name` varchar(24) - Pet name
  - `account_id` int(11) - Owner account ID
  - `char_id` int(11) - Owner character ID
  - `level` smallint(4) - Pet level
  - `egg_id` int(10) - Pet egg item ID
  - `equip` int(10) - Pet equipment
  - `intimate` smallint(9) - Intimacy level
  - `hungry` smallint(9) - Hunger status
  - `rename_flag` tinyint(4) - Rename permission
  - `incubate` int(11) - Incubation status
  - `autofeed` tinyint(2) - Auto-feeding setting

## Mercenary System

### 1. `mercenary` - Mercenary Status
- Purpose: Active mercenary data
- Fields:
  - `mer_id` int(11) - Unique mercenary ID (Primary Key)
  - `char_id` int(11) - Owner character ID
  - `class` mediumint(9) - Mercenary class
  - `hp` int(11) - Current HP
  - `sp` int(11) - Current SP
  - `kill_counter` int(11) - Kill count
  - `life_time` bigint(20) - Remaining contract time

### 2. `mercenary_owner` - Mercenary Contracts
- Purpose: Mercenary ownership data
- Fields:
  - `char_id` int(11) - Character ID (Primary Key)
  - `merc_id` int(11) - Active mercenary ID
  - Faith points:
    - `arch_calls` int(11) - Archer mercenary calls
    - `arch_faith` int(11) - Archer faith points
    - `spear_calls` int(11) - Spearman mercenary calls
    - `spear_faith` int(11) - Spearman faith points
    - `sword_calls` int(11) - Swordman mercenary calls
    - `sword_faith` int(11) - Swordman faith points

## Quest & Achievement System

### 1. `quest` - Quest Progress
- Purpose: Character quest tracking
- Fields:
  - `char_id` int(11) - Character ID (Primary Key part 1)
  - `quest_id` int(10) - Quest ID (Primary Key part 2)
  - `state` enum('0','1','2') - Quest state
  - `time` int(11) - Time limit
  - `count1` to `count3` mediumint(8) - Progress counters

### 2. `achievement` - Achievement System
- Purpose: Character achievement tracking
- Fields:
  - `char_id` int(11) - Character ID (Primary Key part 1)
  - `id` bigint(11) - Achievement ID (Primary Key part 2)
  - `count1` to `count10` int - Progress counters
  - `completed` datetime - Completion time
  - `rewarded` datetime - Reward claim time

## Mail System

### 1. `mail` - Mail Messages
- Purpose: In-game mail system
- Fields:
  - `id` bigint(20) - Unique mail ID (Primary Key)
  - `send_name` varchar(30) - Sender name
  - `send_id` int(11) - Sender character ID
  - `dest_name` varchar(30) - Recipient name
  - `dest_id` int(11) - Recipient character ID
  - `title` varchar(45) - Mail subject
  - `message` varchar(500) - Mail content
  - `time` int(11) - Send time
  - `status` tinyint(2) - Mail status
  - `zeny` int(11) - Attached money
  - `type` smallint(5) - Mail type

### 2. `mail_attachments` - Mail Items
- Purpose: Attached items in mail
- Fields:
  - `id` bigint(20) - Mail ID (Primary Key part 1)
  - `index` smallint(5) - Attachment index (Primary Key part 2)
  - Item data: (Similar structure to inventory items)
    - `nameid`, `amount`, `refine`, `attribute`
    - `card0` to `card3`
    - `option_id0` to `option_id4`
    - `option_val0` to `option_val4`
    - `option_parm0` to `option_parm4`
    - `unique_id`, `bound`, `enchantgrade`

## Market System

### 1. `auction` - Auction House
- Purpose: Item auction system
- Fields:
  - `auction_id` bigint(20) - Unique auction ID (Primary Key)
  - Seller info:
    - `seller_id` int(11) - Seller character ID
    - `seller_name` varchar(30) - Seller name
  - Buyer info:
    - `buyer_id` int(11) - Buyer character ID
    - `buyer_name` varchar(30) - Buyer name
  - `price` int(11) - Current price
  - `buynow` int(11) - Buyout price
  - `hours` smallint(6) - Duration
  - `timestamp` int(11) - Start time
  - Item data: (Similar structure to inventory items)

### 2. `market` - Market Shop
- Purpose: Permanent market shop data
- Fields:
  - `name` varchar(50) - Shop name (Primary Key part 1)
  - `nameid` int(10) - Item ID (Primary Key part 2)
  - `price` INT(11) - Item price
  - `amount` INT(11) - Stock amount
  - `flag` TINYINT(2) - Shop flags

### 3. `vending_items`/`vendings` - Vending Shops
- Purpose: Player shop system
- `vendings` fields:
  - `id` int(10) - Shop ID (Primary Key)
  - `account_id` int(11) - Owner account ID
  - `char_id` int(10) - Owner character ID
  - `sex` enum('F','M') - Character gender
  - `map` varchar(20) - Shop location map
  - `x`, `y` smallint(5) - Shop coordinates
  - `title` varchar(80) - Shop title
  - Direction and status:
    - `body_direction` CHAR(1)
    - `head_direction` CHAR(1)
    - `sit` CHAR(1)
  - `autotrade` tinyint(4) - Autotrade flag
- `vending_items` fields:
  - `vending_id` int(10) - Shop ID (Primary Key part 1)
  - `index` smallint(5) - Item index (Primary Key part 2)
  - `cartinventory_id` int(10) - Cart inventory reference
  - `amount` smallint(5) - Sale quantity
  - `price` int(10) - Item price

## Logging System

### 1. `atcommandlog` - Admin Commands
- Purpose: Tracks admin command usage
- Fields:
  - `atcommand_id` mediumint(9) - Log entry ID (Primary Key)
  - `atcommand_date` datetime - Command execution time
  - `account_id` int(11) - Admin account ID
  - `char_id` int(11) - Admin character ID
  - `char_name` varchar(25) - Admin character name
  - `map` varchar(11) - Map location
  - `command` varchar(255) - Command used

### 2. `chatlog` - Chat Messages
- Purpose: Chat history tracking
- Fields:
  - `id` bigint(20) - Log entry ID (Primary Key)
  - `time` datetime - Message time
  - `type` enum('O','W','P','G','M','C') - Chat type
  - `type_id` int(11) - Channel ID
  - `src_charid` int(11) - Speaker character ID
  - `src_accountid` int(11) - Speaker account ID
  - `src_map` varchar(11) - Speaker location
  - `src_map_x`, `src_map_y` smallint(4) - Speaker coordinates
  - `dst_charname` varchar(25) - Target character name
  - `message` varchar(150) - Chat content

### 3. `picklog` - Item Transactions
- Purpose: Detailed item movement tracking
- Fields:
  - `id` int(11) - Log entry ID (Primary Key)
  - `time` datetime - Transaction time
  - `char_id` int(11) - Character ID
  - `type` enum - Transaction type:
    - 'M' - Monster Drop
    - 'P' - Player Drop/Pick
    - 'L' - Mob Loot
    - 'T' - Trade
    - 'V' - Vending
    - 'S' - Shop
    - 'N' - NPC
    - Many more types...
  - Item data: (Similar structure to inventory items)
  - `map` varchar(11) - Location

### 4. `zenylog` - Currency Log
- Purpose: Money transaction tracking
- Fields:
  - `id` int(11) - Log entry ID (Primary Key)
  - `time` datetime - Transaction time
  - `char_id` int(11) - Character ID
  - `src_id` int(11) - Source ID
  - `type` enum - Transaction type:
    - 'T' - Trade
    - 'V' - Vending
    - 'P' - Player
    - 'M' - Monster
    - 'S' - Shop
    - Other types...
  - `amount` int(11) - Transaction amount
  - `map` varchar(11) - Location

### 5. `mvplog` - MVP Rewards
- Purpose: MVP kill rewards tracking
- Fields:
  - `mvp_id` mediumint(9) - Log entry ID (Primary Key)
  - `mvp_date` datetime - Kill time
  - `kill_char_id` int(11) - Killer character ID
  - `monster_id` smallint(6) - MVP monster ID
  - `prize` int(10) - Item prize ID
  - `mvpexp` bigint(20) - MVP experience
  - `map` varchar(11) - Kill location

## Clan System

### 1. `clan` - Clan Data
- Purpose: Basic clan information
- Fields:
  - `clan_id` int(11) - Unique clan ID (Primary Key)
  - `name` varchar(24) - Clan name
  - `master` varchar(24) - Clan leader
  - `mapname` varchar(24) - Clan map
  - `max_member` smallint(6) - Member limit

### 2. `clan_alliance` - Clan Relations
- Purpose: Clan relationship tracking
- Fields:
  - `clan_id` int(11) - Primary clan ID (Primary Key part 1)
  - `opposition` int(11) - Relationship type
  - `alliance_id` int(11) - Target clan ID (Primary Key part 2)
  - `name` varchar(24) - Alliance clan name

## Variable Storage

### 1. `mapreg` - Script Variables
- Purpose: Global script variable storage
- Fields:
  - `varname` varchar(32) - Variable name (Primary Key part 1)
  - `index` int(11) - Array index (Primary Key part 2)
  - `value` varchar(255) - Variable value

### 2. `acc_reg_num`/`acc_reg_str` - Account Variables
- Purpose: Account-specific variables
- Fields:
  - `account_id` int(11) - Account ID (Primary Key part 1)
  - `key` varchar(32) - Variable name (Primary Key part 2)
  - `index` int(11) - Array index (Primary Key part 3)
  - `value` - Variable value (bigint(11) for num, varchar(254) for str)

### 3. `char_reg_num`/`char_reg_str` - Character Variables
- Purpose: Character-specific variables
- Fields:
  - `char_id` int(11) - Character ID (Primary Key part 1)
  - `key` varchar(32) - Variable name (Primary Key part 2)
  - `index` int(11) - Array index (Primary Key part 3)
  - `value` - Variable value (bigint(11) for num, varchar(254) for str)