#include "TileData.h"
#include "json/json.hpp"
#include <fstream>

using namespace Fastboi;
using namespace CS;
using nlohmann::json;

std::vector<TileData> tileData;

namespace CS {
    void from_json(const json& j, TileData& td) {
        unsigned int tempId;
        std::vector<int> size;
        std::vector<int> corner;

        j.at("id").get_to(tempId);
        j.at("size").get_to(size);
        j.at("corner").get_to(corner);

        td.id = static_cast<TileID>(tempId);
        td.cutout = Rect(corner[0], corner[1], size[0], size[1]);
    }
}

const TileData& TileData::Get(TileID id) {
    return tileData[static_cast<decltype(tileData)::size_type>(id)];
}

void CS::InitTileData() {
    std::ifstream file("data/tile_data.json");
    json js;
    
    file >> js;

    auto& tiles = js["tiles"];

    tileData = std::vector<TileData>(tiles.size());

    for (auto& x : tiles.items()) {
        std::string name = x.value()["name"];
        printf("Name: %s\n", name.c_str());

        TileData td = x.value().get<TileData>();

        tileData[static_cast<decltype(tileData)::size_type>(td.id)] = td;
    }
}