#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* OpenRCT2 is the work of many authors, a full list can be found in contributors.md
* For more information, visit https://github.com/OpenRCT2/OpenRCT2
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* A full copy of the GNU General Public License can be found in licence.txt
*****************************************************************************/
#pragma endregion

#include "../core/Guard.hpp"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../ride/Track.h"
#include "Banner.h"
#include "LargeScenery.h"
#include "TileElement.h"
#include "Scenery.h"

uint8 tile_element_get_scenery_quadrant(const rct_tile_element * element)
{
    return (element->type & TILE_ELEMENT_QUADRANT_MASK) >> 6;
}

sint32 tile_element_get_type(const rct_tile_element * element)
{
    return element->type & TILE_ELEMENT_TYPE_MASK;
}

sint32 tile_element_get_direction(const rct_tile_element * element)
{
    return element->type & TILE_ELEMENT_DIRECTION_MASK;
}

sint32 tile_element_get_direction_with_offset(const rct_tile_element * element, uint8 offset)
{
    return ((element->type & TILE_ELEMENT_DIRECTION_MASK) + offset) & TILE_ELEMENT_DIRECTION_MASK;
}

bool tile_element_is_ghost(const rct_tile_element * element)
{
    return element->flags & TILE_ELEMENT_FLAG_GHOST;
}

bool tile_element_is_underground(rct_tile_element * tileElement)
{
    do {
        tileElement++;
        if (tile_element_is_last_for_tile(tileElement - 1))
            return false;
    } while (tile_element_get_type(tileElement) != TILE_ELEMENT_TYPE_SURFACE);
    return true;
}

bool tile_element_is_last_for_tile(const rct_tile_element *element)
{
    return (element->flags & TILE_ELEMENT_FLAG_LAST_TILE) != 0;
}

sint32 tile_element_get_banner_index(rct_tile_element * tileElement)
{
    rct_scenery_entry* sceneryEntry;

    switch (tile_element_get_type(tileElement)) {
    case TILE_ELEMENT_TYPE_LARGE_SCENERY:
        sceneryEntry = get_large_scenery_entry(scenery_large_get_type(tileElement));
        if (sceneryEntry->large_scenery.scrolling_mode == 0xFF)
            return BANNER_INDEX_NULL;

        return scenery_large_get_banner_id(tileElement);
    case TILE_ELEMENT_TYPE_WALL:
        sceneryEntry = get_wall_entry(tileElement->properties.wall.type);
        if (sceneryEntry == nullptr || sceneryEntry->wall.scrolling_mode == 0xFF)
            return BANNER_INDEX_NULL;

        return tileElement->properties.wall.banner_index;
    case TILE_ELEMENT_TYPE_BANNER:
        return tileElement->properties.banner.index;
    default:
        return BANNER_INDEX_NULL;
    }
}

void tile_element_set_banner_index(rct_tile_element * tileElement, sint32 bannerIndex)
{
    switch (tile_element_get_type(tileElement))
    {
    case TILE_ELEMENT_TYPE_WALL:
        tileElement->properties.wall.banner_index = (uint8)bannerIndex;
        break;
    case TILE_ELEMENT_TYPE_LARGE_SCENERY:
        scenery_large_set_banner_id(tileElement, (uint8)bannerIndex);
        break;
    case TILE_ELEMENT_TYPE_BANNER:
        tileElement->properties.banner.index = (uint8)bannerIndex;
        break;
    default:
        log_error("Tried to set banner index on unsuitable tile element!");
        Guard::Assert(false);
    }
}

void tile_element_remove_banner_entry(rct_tile_element * tileElement)
{
    sint32 bannerIndex = tile_element_get_banner_index(tileElement);
    if (bannerIndex == BANNER_INDEX_NULL)
        return;

    rct_banner* banner = &gBanners[bannerIndex];
    if (banner->type != BANNER_NULL) {
        window_close_by_number(WC_BANNER, bannerIndex);
        banner->type = BANNER_NULL;
        user_string_free(banner->string_idx);
    }
}

uint8 tile_element_get_ride_index(const rct_tile_element * tileElement)
{
    switch (tile_element_get_type(tileElement))
    {
    case TILE_ELEMENT_TYPE_TRACK:
        return track_element_get_ride_index(tileElement);
    case TILE_ELEMENT_TYPE_ENTRANCE:
        return tileElement->properties.entrance.ride_index;
    case TILE_ELEMENT_TYPE_PATH:
        return tileElement->properties.path.ride_index;
    default:
        return 0xFF;
    }
}
