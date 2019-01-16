//////////////////////////////////////////////////////////////////////
// This file is part of Remere's Map Editor
//////////////////////////////////////////////////////////////////////
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//////////////////////////////////////////////////////////////////////
// $URL: http://svn.rebarp.se/svn/RME/trunk/source/items.hpp $
// $Id: items.hpp 310 2010-02-26 18:03:48Z admin $

#include "main.h"

#include "materials.h"
#include "gui.h"
#include <string.h> // memcpy

#include "items.h"
#include "item.h"
#include "pugicast.h"
#include "script.h"

ItemDatabase g_items;

ItemType::ItemType() :
hasDisguise(false),
disguiseId(0),

	sprite(nullptr),
	id(0),
	clientID(0),
	brush(nullptr),
	doodad_brush(nullptr),
	raw_brush(nullptr),
	is_metaitem(false),
	has_raw(false),
	in_other_tileset(false),
	group(ITEM_GROUP_NONE),
	type(ITEM_TYPE_NONE),
	volume(0),
	maxTextLen(0),
	//writeOnceItemID(0),
	ground_equivalent(0),
	border_group(0),
	has_equivalent(false),
	wall_hate_me(false),
	name(""),
	description(""),
	weight(0.0f),
	attack(0),
	defense(0),
	armor(0),
	charges(0),
	client_chargeable(false),
	extra_chargeable(false),
	ignoreLook(false),

	isHangable(false),
	hookEast(false),
	hookSouth(false),
	canReadText(false),
	canWriteText(false),
	replaceable(true),
	decays(false),
	stackable(false),
	moveable(true),
	alwaysOnBottom(false),
	pickupable(false),
	rotable(false),
	isBorder(false),
	isOptionalBorder(false),
	isWall(false),
	isBrushDoor(false),
	isOpen(false),
	isTable(false),
	isCarpet(false),

	floorChangeDown(false),
	floorChangeNorth(false),
	floorChangeSouth(false),
	floorChangeEast(false),
	floorChangeWest(false),
	floorChange(false),

	unpassable(false),
	blockPickupable(false),
	blockMissiles(false),
	blockPathfinder(false),
	hasElevation(false),

	alwaysOnTopOrder(0),
	rotateTo(0),
	border_alignment(BORDER_NONE)
{
	////
}

ItemType::~ItemType()
{
	////
}

bool ItemType::isFloorChange() const
{
	return floorChange || floorChangeDown || floorChangeNorth || floorChangeSouth || floorChangeEast || floorChangeWest;
}

ItemDatabase::ItemDatabase() :
	// Version information
	MajorVersion(0),
	MinorVersion(0),
	BuildNumber(0),

	// Count of GameSprite types
	item_count(0),
	effect_count(0),
	monster_count(0),
	distance_count(0),

	minclientID(0),
	maxclientID(0),

	max_item_id(0)
{
	////
}

ItemDatabase::~ItemDatabase()
{
	clear();
}

void ItemDatabase::clear()
{
	for(uint32_t i = 0; i < items.size(); i++) {
		delete items[i];
		items.set(i, nullptr);
	}
}

bool ItemDatabase::loadFromOtbVer1()
{
	ItemType* object = nullptr;
	MinorVersion = CLIENT_VERSION_770;
	MajorVersion = CLIENT_VERSION_770;

	ScriptReader script;
	if (!script.open("data/items.srv")) {
		return false;
		}

	std::string identifier;
	uint16_t id = 0;
	while (true) {
		script.nextToken();
		if (script.Token == ENDOFFILE) {
				break;
			}

		if (script.Token != IDENTIFIER) {
			script.error("Identifier expected");
						return false;
					}

		std::string identifier = script.getIdentifier();
		script.readSymbol('=');

		if (identifier == "typeid") {
			if (object) {
				items.set(object->id, object);
				if (object->hasDisguise) {
					object->sprite = static_cast<GameSprite*>(g_gui.gfx.getSprite(object->disguiseId));
				}
				else {
					object->sprite = static_cast<GameSprite*>(g_gui.gfx.getSprite(id));
				}
					}

			object = newd ItemType();
			id = script.readNumber();
			object->id = id;
			object->clientID = id;
			max_item_id = object->id;
				}
		else if (identifier == "name") {
			object->name = script.readString();
					}
		else if (identifier == "description") {
			object->description = script.readString();
				}
		else if (identifier == "flags") {
			script.readSymbol('{');
			while (true) {
				while (true) {
					script.nextToken();
					if (script.Token == SPECIAL) {
						break;
					}

					identifier = script.getIdentifier();

					if (identifier == "bank") {
						object->group = ITEM_GROUP_GROUND;
					}
					else if (identifier == "clip") {
						//object->alwaysOnTop = true;
						object->alwaysOnBottom = true;
						object->alwaysOnTopOrder = 1;
					}
					else if (identifier == "bottom") {
						//object->alwaysOnTop = true;
						object->alwaysOnBottom = true;
						object->alwaysOnTopOrder = 2;
					}
					else if (identifier == "top") {
						//object->alwaysOnTop = true;
						object->alwaysOnBottom = true;
						object->alwaysOnTopOrder = 3;
					}
					else if (identifier == "container") {
						object->type = ITEM_TYPE_CONTAINER;
						object->group = ITEM_GROUP_CONTAINER;
					}
					else if (identifier == "chest") {
						object->type = ITEM_TYPE_CONTAINER;
						object->group = ITEM_GROUP_CONTAINER;
						object->volume = 1;
					}
					else if (identifier == "cumulative") {
						object->stackable = true;
					}
					else if (identifier == "changeuse") {
						//object->changeUse = true;
					}
					else if (identifier == "forceuse") {
						//object->forceUse = true;
					}
					else if (identifier == "key") {
						object->type = ITEM_TYPE_KEY;
						object->group = ITEM_GROUP_KEY;
					}
					else if (identifier == "door") {
						object->type = ITEM_TYPE_DOOR;
					}
					else if (identifier == "bed") {
						object->type = ITEM_TYPE_BED;
					}
					else if (identifier == "rune") {
						//object->type = ITEM_TYPE_RUNE;
						object->client_chargeable = true;
					}
					else if (identifier == "depotlocker") {
						object->type = ITEM_TYPE_DEPOT;
					}
					else if (identifier == "mailbox") {
						object->type = ITEM_TYPE_MAILBOX;
					}
					else if (identifier == "allowdistread") {
						object->allowDistRead = true;
					}
					else if (identifier == "text") {
						object->canReadText = true;
					}
					else if (identifier == "write") {
						object->canWriteText = true;
					}
					else if (identifier == "writeonce") {
						object->canWriteText = true;
						//object->writeOnceItemId = id;
					}
					else if (identifier == "fluidcontainer") {
						object->group = ITEM_GROUP_FLUID;
					}
					else if (identifier == "splash") {
						object->group = ITEM_GROUP_SPLASH;
					}
					else if (identifier == "unpass") {
						object->unpassable = true;
					}
					else if (identifier == "unmove") {
						object->moveable = false;
					}
					else if (identifier == "unthrow") {
						object->blockMissiles = true;
					}
					else if (identifier == "unlay") {
						//object->allowPickupable = false;
					}
					else if (identifier == "avoid") {
						object->blockPathfinder = true;
					}
					else if (identifier == "magicfield") {
						object->type = ITEM_TYPE_MAGICFIELD;
						object->group = ITEM_GROUP_MAGICFIELD;
					}
					else if (identifier == "restrictlevel") {
						//object->wieldInfo |= WIELDINFO_LEVEL;
					}
					else if (identifier == "restrictprofession") {
						//object->wieldInfo |= WIELDINFO_VOCREQ;
					}
					else if (identifier == "take") {
						object->pickupable = true;
					}
					else if (identifier == "hang") {
						object->isHangable = true;
					}
					else if (identifier == "hooksouth") {
						object->hookSouth = true;
					}
					else if (identifier == "hookeast") {
						object->hookEast = true;
					}
					else if (identifier == "rotate") {
						object->rotable = true;
					}
					else if (identifier == "destroy") {
						//object->destroy = true;
					}
					else if (identifier == "corpse") {
						//object->corpse = true;
					}
					else if (identifier == "expire") {
						//object->stopTime = false;
					}
					else if (identifier == "expirestop") {
						//object->stopTime = true;
					}
					else if (identifier == "weapon") {
						object->group = ITEM_GROUP_WEAPON;
					}
					else if (identifier == "shield") {
						//object->weaponType = WEAPON_SHIELD;
					}
					else if (identifier == "distance") {
						//object->weaponType = WEAPON_DISTANCE;
					}
					else if (identifier == "wand") {
						//object->weaponType = WEAPON_WAND;
					}
					else if (identifier == "ammo") {
						//object->weaponType = WEAPON_AMMO;
					}
					else if (identifier == "armor") {
						object->group = ITEM_GROUP_ARMOR;
					}
					else if (identifier == "height") {
						//object->hasHeight = true;
					}
					else if (identifier == "disguise") {
						//object->disguise = true;
						object->hasDisguise = true;
					}
					else if (identifier == "showdetail") {
						//object->showDuration = true;
					}
					else if (identifier == "noreplace") {
						object->replaceable = false;
					}
				}

				if (script.getSpecial() == '}') {
						break;
					}

				if (script.Token != SPECIAL || script.getSpecial() != ',') {
					continue;
				}
			}
				}
		else if (identifier == "attributes") {
			script.readSymbol('{');
			while (true) {
				while (true) {
					script.nextToken();
					if (script.Token == SPECIAL) {
						break;
					}

					identifier = script.getIdentifier();
					script.readSymbol('=');

					if (identifier == "waypoints") {
						//object->speed = script.readNumber();
						script.readNumber();
					}
					else if (identifier == "capacity") {
						object->volume = script.readNumber();
				}
					else if (identifier == "changetarget") {
						//object->transformUseTo = script.readNumber();
						script.readNumber();
					}
					else if (identifier == "nutrition") {
						//object->nutrition = script.readNumber();
						script.readNumber();
					}
					else if (identifier == "maxlength") {
						object->maxTextLen = script.readNumber();
				}
					else if (identifier == "fluidsource") {
						//object->fluidSource = getFluidType(script.readIdentifier());
						script.readIdentifier();
					}
					else if (identifier == "avoiddamagetypes") {
						//object->combatType = getCombatType(script.readIdentifier());
						script.readIdentifier();
				}
					else if (identifier == "minimumlevel") {
						//object->minReqLevel = script.readNumber();
						script.readNumber();
					}
					else if (identifier == "beddirection") {
						//object->bedPartnerDir = getDirection(script.readIdentifier());
						script.readIdentifier();
					}
					else if (identifier == "bedtarget") {
						//object->bedTarget = script.readNumber();
						script.readNumber();
				}
					else if (identifier == "bedfree") {
						//object->bedFree = script.readNumber();
						script.readNumber();
					}
					else if (identifier == "weight") {
						object->weight = script.readNumber();
					}
					else if (identifier == "rotatetarget") {
						object->rotateTo = script.readNumber();
				}
					else if (identifier == "destroytarget") {
						//object->destroyTarget = script.readNumber();
						script.readNumber();
					}
					else if (identifier == "slottype") {
						identifier = script.readIdentifier();
						/*if (identifier == "head") {
							object->slotPosition |= SLOTP_HEAD;
					}
						else if (identifier == "body") {
							object->slotPosition |= SLOTP_ARMOR;
					}
						else if (identifier == "legs") {
							object->slotPosition |= SLOTP_LEGS;
				}
						else if (identifier == "feet") {
							object->slotPosition |= SLOTP_FEET;
				}
						else if (identifier == "backpack") {
							object->slotPosition |= SLOTP_BACKPACK;
			}
						else if (identifier == "twohanded") {
							object->slotPosition |= SLOTP_TWO_HAND;
		}
						else if (identifier == "righthand") {
							object->slotPosition &= ~SLOTP_LEFT;
			}
						else if (identifier == "lefthand") {
							object->slotPosition &= ~SLOTP_RIGHT;
		}
						else if (identifier == "necklace") {
							object->slotPosition |= SLOTP_NECKLACE;
	}
						else if (identifier == "ring") {
							object->slotPosition |= SLOTP_RING;
}
						else if (identifier == "ammo") {
							object->slotPosition |= SLOTP_AMMO;
		}
						else if (identifier == "hand") {
							object->slotPosition |= SLOTP_HAND;
			}
						else {
							script.error("Unknown slot position");
						return false;
						}*/
					}
					else if (identifier == "speedboost") {
						//object->getAbilities().speed = script.readNumber();
						script.readNumber();
					}
					else if (identifier == "fistboost") {
						//object->getAbilities().skills[SKILL_FIST] = script.readNumber();
						script.readNumber();
					}
					else if (identifier == "swordboost") {
						//object->getAbilities().skills[SKILL_SWORD] = script.readNumber();
						script.readNumber();
					}
					else if (identifier == "clubboost") {
						//object->getAbilities().skills[SKILL_CLUB] = script.readNumber();
						script.readNumber();
					}
					else if (identifier == "axeboost") {
						//object->getAbilities().skills[SKILL_AXE] = script.readNumber();
						script.readNumber();
					}
					else if (identifier == "shieldboost") {
						//object->getAbilities().skills[SKILL_SHIELD] = script.readNumber();
						script.readNumber();
					}
					else if (identifier == "suppressdrunk") {
						/*if (script.readNumber()) {
							object->getAbilities().conditionSuppressions |= CONDITION_DRUNK;
						}*/
						script.readNumber();
					}
					else if (identifier == "invisible") {
						/*if (script.readNumber()) {
							object->getAbilities().invisible = true;
						}*/
						script.readNumber();
					}
					else if (identifier == "manashield") {
						/*if (script.readNumber()) {
							object->getAbilities().manaShield = true;
						}*/
						script.readNumber();
					}
					else if (identifier == "healthticks") {
						/*Abilities& abilities = object->getAbilities();
						abilities.regeneration = true;
						abilities.healthTicks = script.readNumber();*/
						script.readNumber();
					}
					else if (identifier == "healthgain") {
						/*Abilities& abilities = object->getAbilities();
						abilities.regeneration = true;
						abilities.healthTicks = script.readNumber();*/
						script.readNumber();
					}
					else if (identifier == "manaticks") {
						/*Abilities& abilities = object->getAbilities();
						abilities.regeneration = true;
						abilities.healthTicks = script.readNumber();*/
						script.readNumber();
					}
					else if (identifier == "managain") {
						/*Abilities& abilities = object->getAbilities();
						abilities.regeneration = true;
						abilities.healthTicks = script.readNumber();*/
						script.readNumber();
					}
					else if (identifier == "absorbmagic") {
						/*Abilities& abilities = object->getAbilities();
						abilities.regeneration = true;
						abilities.healthTicks = script.readNumber();*/
						script.readNumber();
					}
					else if (identifier == "absorbenergy") {
						/*Abilities& abilities = object->getAbilities();
						abilities.regeneration = true;
						abilities.healthTicks = script.readNumber();*/
						script.readNumber();
					}
					else if (identifier == "absorbfire") {
						script.readNumber();
				}
					else if (identifier == "absorbpoison") {
						script.readNumber();
					}
					else if (identifier == "absorblifedrain") {
						script.readNumber();
				}
					else if (identifier == "absorbmanadrain") {
						script.readNumber();
					}
					else if (identifier == "absorbdrown") {
						script.readNumber();
				}
					else if (identifier == "absorbphysical") {
						script.readNumber();
					}
					else if (identifier == "absorbhealing") {
						script.readNumber();
				}
					else if (identifier == "absorbundefined") {
						script.readNumber();
					}
					else if (identifier == "brightness") {
						script.readNumber();
					}
					else if (identifier == "lightcolor") {
						script.readNumber();
				}
					else if (identifier == "totalexpiretime") {
						script.readNumber();
				}
					else if (identifier == "expiretarget") {
						script.readNumber();
			}
					else if (identifier == "totaluses") {
						object->charges = script.readNumber();
		}
					else if (identifier == "weapontype") {
						identifier = script.readIdentifier();
						/*if (identifier == "sword") {
							object->weaponType = WEAPON_SWORD;
			}
						else if (identifier == "club") {
							object->weaponType = WEAPON_CLUB;
		}
						else if (identifier == "axe") {
							object->weaponType = WEAPON_AXE;
	}
						else if (identifier == "shield") {
							object->weaponType = WEAPON_SHIELD;
}
						else if (identifier == "distance") {
							object->weaponType = WEAPON_DISTANCE;
		}
						else if (identifier == "wand") {
							object->weaponType = WEAPON_WAND;
		}
						else if (identifier == "ammunition") {
							object->weaponType = WEAPON_AMMO;
			}
						else {
							script.error("Unknown weapon type");
						return false;
						}*/
					}
					else if (identifier == "attack") {
						object->attack = script.readNumber();
					}
					else if (identifier == "defense") {
						object->defense = script.readNumber();
				}
					else if (identifier == "range") {
						//object->shootRange = static_cast<uint8_t>(script.readNumber());
						script.readNumber();
					}
					else if (identifier == "ammotype") {
						/*object->ammoType = getAmmoType(script.readIdentifier());
						if (object->ammoType == AMMO_NONE) {
							script.error("Unknown ammo type");
						return false;
						}*/
						script.readIdentifier();
					}
					else if (identifier == "missileeffect") {
						//object->shootType = getShootType(script.readIdentifier());
						script.readIdentifier();
					}
					else if (identifier == "armorvalue") {
						object->armor = script.readNumber();
					}
					else if (identifier == "disguisetarget") {
						object->disguiseId = script.readNumber();
					}
					else if (identifier == "equiptarget") {
					//	object->transformEquipTo = script.readNumber();
						script.readNumber();
					}
					else if (identifier == "deequiptarget") {
					//	object->transformDeEquipTo = script.readNumber();
						script.readNumber();
				}
					/*else {
						script.error("Unknown attribute");
						return false;
					}*/
					}

				if (script.getSpecial() == '}') {
					break;
				}

				if (script.Token != SPECIAL || script.getSpecial() != ',') {
					continue;
				}
					}
				}
		else if (identifier == "magicfield") {
			script.readSymbol('{');

			while (true) {
				while (true) {
					script.nextToken();
					if (script.Token == SPECIAL) {
						break;
					}

					identifier = script.getIdentifier();
					script.readSymbol('=');

					if (identifier == "type") {
						identifier = script.readIdentifier();
					}
					else if (identifier == "ticks") {
						script.readNumber();
					}
					else if (identifier == "count") {
						script.readNumber();
				}
					else if (identifier == "start") {
						script.readNumber();
				}
					else if (identifier == "damage") {
						script.readNumber();
			}
		}

				if (script.getSpecial() == '}') {
					break;
			}

				if (script.Token != SPECIAL || script.getSpecial() != ',') {
					continue;
		}
	}
}
	}

	script.close();
	return true;
		}

bool ItemDatabase::loadFromOtbVer2()
{
	return loadFromOtbVer1();
	}

bool ItemDatabase::loadFromOtbVer3() {
	return loadFromOtbVer1();
	}

bool ItemDatabase::loadFromOtb()
{
	return loadFromOtbVer1();
}

bool ItemDatabase::loadItemFromGameXml(pugi::xml_node itemNode, int id)
{
	ClientVersionID clientVersion = g_gui.GetCurrentVersionID();
	if(clientVersion < CLIENT_VERSION_980 && id > 20000 && id < 20100) {
		itemNode = itemNode.next_sibling();
		return true;
	}
	else if (id > 30000 && id < 30100) {
		itemNode = itemNode.next_sibling();
		return true;
	}

	ItemType& it = getItemType(id);

	it.name = itemNode.attribute("name").as_string();
	it.editorsuffix = itemNode.attribute("editorsuffix").as_string();

	pugi::xml_attribute attribute;
	for(pugi::xml_node itemAttributesNode = itemNode.first_child(); itemAttributesNode; itemAttributesNode = itemAttributesNode.next_sibling()) {
		if(!(attribute = itemAttributesNode.attribute("key"))) {
			continue;
		}

		std::string key = attribute.as_string();
		to_lower_str(key);
		if(key == "type") {
			if(!(attribute = itemAttributesNode.attribute("value"))) {
				continue;
			}

			std::string typeValue = attribute.as_string();
			to_lower_str(key);
			if(typeValue == "magicfield") {
				it.group = ITEM_GROUP_MAGICFIELD;
				it.type = ITEM_TYPE_MAGICFIELD;
			}
			else if (typeValue == "key") {
				it.type = ITEM_TYPE_KEY;
			}
			else if (typeValue == "depot") {
				it.type = ITEM_TYPE_DEPOT;
			}
			else if (typeValue == "teleport") {
				it.type = ITEM_TYPE_TELEPORT;
			}
			else if (typeValue == "bed") {
				it.type = ITEM_TYPE_BED;
			}
			else if (typeValue == "door") {
				it.type = ITEM_TYPE_DOOR;
			}
			else {
				// We ignore many types, no need to complain
				//warnings.push_back("items.xml: Unknown type " + typeValue);
			}
		}
		else if (key == "name") {
			if((attribute = itemAttributesNode.attribute("value"))) {
				it.name = attribute.as_string();
			}
		}
		else if (key == "description") {
			if((attribute = itemAttributesNode.attribute("value"))) {
				it.description = attribute.as_string();
			}
		}
		else if (key == "runespellName") {
			/*if((attribute = itemAttributesNode.attribute("value"))) {
				it.runeSpellName = attribute.as_string();
			}*/
		}
		else if (key == "weight") {
			if((attribute = itemAttributesNode.attribute("value"))) {
				it.weight = pugi::cast<int32_t>(attribute.value()) / 100.f;
			}
		}
		else if (key == "armor") {
			if((attribute = itemAttributesNode.attribute("value"))) {
				it.armor = pugi::cast<int32_t>(attribute.value());
			}
		}
		else if (key == "defense") {
			if((attribute = itemAttributesNode.attribute("value"))) {
				it.defense = pugi::cast<int32_t>(attribute.value());
			}
		}
		else if (key == "rotateto") {
			if((attribute = itemAttributesNode.attribute("value"))) {
				it.rotateTo = pugi::cast<int32_t>(attribute.value());
			}
		}
		else if (key == "containersize") {
			if((attribute = itemAttributesNode.attribute("value"))) {
				it.volume = pugi::cast<int32_t>(attribute.value());
			}
		}
		else if (key == "readable") {
			if((attribute = itemAttributesNode.attribute("value"))) {
				it.canReadText = attribute.as_bool();
			}
		}
		else if (key == "writeable") {
			if((attribute = itemAttributesNode.attribute("value"))) {
				it.canWriteText = it.canReadText = attribute.as_bool();
			}
		}
		else if (key == "decayto") {
			it.decays = true;
		}
		else if (key == "maxtextlen" || key == "maxtextlength") {
			if((attribute = itemAttributesNode.attribute("value"))) {
				it.maxTextLen = pugi::cast<int32_t>(attribute.value());
				it.canReadText = it.maxTextLen > 0;
			}
		}
		else if (key == "writeonceitemid") {
			/*if((attribute = itemAttributesNode.attribute("value"))) {
				it.writeOnceItemId = pugi::cast<int32_t>(attribute.value());
			}*/
		}
		else if (key == "allowdistread") {
			if((attribute = itemAttributesNode.attribute("value"))) {
				it.allowDistRead = attribute.as_bool();
			}
		}
		else if (key == "charges") {
			if((attribute = itemAttributesNode.attribute("value"))) {
				it.charges = pugi::cast<int32_t>(attribute.value());
				it.extra_chargeable = true;
			}
		}
	}
	return true;
}

bool ItemDatabase::loadFromGameXml(const FileName& identifier, wxString& error, wxArrayString& warnings)
{
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(identifier.GetFullPath().mb_str());
	if(!result) {
		error = wxT("Could not load items.xml (Syntax error?)");
		return false;
	}

	pugi::xml_node node = doc.child("items");
	if(!node) {
		error = wxT("items.xml, invalid root node.");
		return false;
	}

	for(pugi::xml_node itemNode = node.first_child(); itemNode; itemNode = itemNode.next_sibling()) {
		if(as_lower_str(itemNode.name()) != "item") {
			continue;
		}

		int32_t fromId = pugi::cast<int32_t>(itemNode.attribute("fromid").value());
		int32_t toId = pugi::cast<int32_t>(itemNode.attribute("toid").value());
		if(pugi::xml_attribute attribute = itemNode.attribute("id")) {
			fromId = toId = pugi::cast<int32_t>(attribute.value());
		}

		if(fromId == 0 || toId == 0) {
			error = wxT("Could not read item id from item node.");
			return false;
		}

		for(int32_t id = fromId; id <= toId; ++id) {
			if(!loadItemFromGameXml(itemNode, id)) {
				return false;
			}
		}
	}
	return true;
}

bool ItemDatabase::loadMetaItem(pugi::xml_node node)
{
	if(pugi::xml_attribute attribute = node.attribute("id")) {
		int32_t id = pugi::cast<int32_t>(attribute.value());
		if(items[id]) {
			//std::cout << "Occupied ID " << id << " : " << items[id]->id << ":" << items[id]->name << std::endl;
			return false;
		}
		items.set(id, newd ItemType());
		items[id]->is_metaitem = true;
		items[id]->id = id;
	}
	else {
		return false;
	}
	return true;
}

ItemType& ItemDatabase::getItemType(int id)
{
	ItemType* it = items[id];
	if(it)
		return *it;
	else {
		static ItemType dummyItemType; // use this for invalid ids
		return dummyItemType;
	}
}

bool ItemDatabase::typeExists(int id) const
{
	ItemType* it = items[id];
	return it != nullptr;
}
