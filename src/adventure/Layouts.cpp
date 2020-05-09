#include "Layouts.h"

using namespace Adventure;
using namespace Layouts;
using namespace Fastboi;
using namespace Fastboi::Components;

using L = Layouts::Layout_t;
using VI = Vec<int>;

Vec<int> Layout_t::GetCoords(const Vec<int>& goldCastleCoords) const {
    return gcOffset + goldCastleCoords;
};

//! All coords are relative to the gold castle
const Layout_t Layouts::goldCastle { 
    .tiles{
        "WWWWWOOOOOOOOOOWWWWW",
        "WOOOOWOOOOOOOOWOOOOW",
        "WOOOOWOOOOOOOOWOOOOW",
        "WOOOOWOOOOOOOOWOOOOW",
        "WOOOOWOOOOOOOOWOOOOW",
        "WOOOOOWOOOOOOWOOOOOW",
        "WOOOOOWWWWWWWWOOOOOW",
        "WOOOOOWWWOOWWWOOOOOW",
        "WOOOOOWWWOOWWWOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WWWWWWWWOOOOWWWWWWWW"
    }
    , .gcOffset{0, 0}
    , .color{210, 210, 64, 255}
};

const Layout_t Layouts::goldCastleInside {
    .tiles{
        "WWWWWWWWWWWWWWWWWWWW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WWWWWWWWOOOOWWWWWWWW"
    }
    , .gcOffset{0, -1}
    , .color{210, 210, 64, 255}
};

const Layout_t Layouts::gcSouth {
    .tiles{
        "WWWWWWWWOOOOWWWWWWWW",
        "OOOOOOOOOOOOOOOOOOOO",
        "OOOOOOOOOOOOOOOOOOOO",
        "OOOOOOOOOOOOOOOOOOOO",
        "OOOOOOOOOOOOOOOOOOOO",
        "OOOOOOOOOOOOOOOOOOOO",
        "OOOOOOOOOOOOOOOOOOOO",
        "OOOOOOOOOOOOOOOOOOOO",
        "OOOOOOOOOOOOOOOOOOOO",
        "OOOOOOOOOOOOOOOOOOOO",
        "OOOOOOOOOOOOOOOOOOOO",
        "WWWWWWWWWWWWWWWWWWWW"
    }
    , .gcOffset{0, 1}
    , .color{92, 186, 92, 255}
};

const Layout_t Layouts::gcSoutheast {
    .tiles{
        "WWWWWWWWWWWWWWWWWWWW",
        "OOOOOOOOOOOOOOOOOOOW",
        "OOOOOOOOOOOOOOOOOOOW",
        "OOOOOOOOOOOOOOOOOOOW",
        "OOOOOOOOOOOOOOOOOOOW",
        "OOOOOOOOOOOOOOOOOOOW",
        "OOOOOOOOOOOOOOOOOOOW",
        "OOOOOOOOOOOOOOOOOOOW",
        "OOOOOOOOOOOOOOOOOOOW",
        "OOOOOOOOOOOOOOOOOOOW",
        "OOOOOOOOOOOOOOOOOOOW",
        "WWWWWWWWOOOOWWWWWWWW"
    }
    , .gcOffset{1, 1}
    , .color{160, 171, 79, 255}
};

const Layout_t Layouts::gcSouthwest {
    .tiles{
        "WWWWWWWWOOOOWWWWWWWW",
        "WOOOOOOOOOOOOOOOOOOO",
        "WOOOOOOOOOOOOOOOOOOO",
        "WOOOOOOOOOOOOOOOOOOO",
        "WOOOOOOOOOOOOOOOOOOO",
        "WOOOOOOOOOOOOOOOOOOO",
        "WOOOOOOOOOOOOOOOOOOO",
        "WOOOOOOOOOOOOOOOOOOO",
        "WOOOOOOOOOOOOOOOOOOO",
        "WOOOOOOOOOOOOOOOOOOO",
        "WOOOOOOOOOOOOOOOOOOO",
        "WWWWWWWWWWWWWWWWWWWW"
    }
    , .gcOffset{-1, 1}
    , .color{135, 183, 84, 255}
};

const Layout_t Layouts::greenDragonRm {
    .tiles{
        "WWWWWWWWOOOOWWWWWWWW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WWWWWWWWWWWWWWWWWWWW"
    }
    , .gcOffset{1, 2}
    , .color{198, 108, 58, 255}
};

constexpr ColorComp mazeBlue(66, 72, 200, 255);

const Layout_t Layouts::mazeU {
    .tiles{
        "WWWWOWOWOWWOWOWOWWWW",
        "OOOWOWOWOOOOWOWOWOOO",
        "OOOWOWOWOOOOWOWOWOOO",
        "WWOWOWOWWWWWWOWOWOWW",
        "WWOWOWOWWWWWWOWOWOWW",
        "OOOWOWOOOOOOOOWOWOOO",
        "OOOWOWOOOOOOOOWOWOOO",
        "WWWWOWWWWWWWWWWOWWWW",
        "WWWWOWWWWWWWWWWOWWWW",
        "OOOOOOOOOOOOOOOOOOOO",
        "OOOOOOOOOOOOOOOOOOOO",
        "WWWWWWWWOOOOWWWWWWWW"
    }
    , .gcOffset{-1, 0}
    , .color{mazeBlue}
    , .neighbors{ 
        VI{-2, 1} // Up to mazeHumps
        , L::dfltLeft // Right to mazeSwirls
        , L::dfltDown // Down to gcSouthwest
        , L::dfltLeft // Left to mazeSwirls
    }
};

const Layout_t Layouts::mazeSwirls {
    .tiles{
        "WWOWOWWWWOOWWWWOWOWW",
        "OOOWOOOWWOOWWOOOWOOO",
        "OOOWOOOWWOOWWOOOWOOO",
        "WWWWWWOWWOOWWOWWWWWW",
        "WWWWWWOWWOOWWOWWWWWW",
        "OOOOOWOWWOOWWOWOOOOO",
        "OOOOOWOWWOOWWOWOOOOO",
        "WWOWOWOWWOOWWOWOWOWW",
        "WWOWOWOWWOOWWOWOWOWW",
        "OOOWOWOWOOOOWOWOWOOO",
        "OOOWOWOWOOOOWOWOWOOO",
        "WWWWOWOWOOOOWOWOWWWW"
    }
    , .gcOffset{-2, 0}
    , .color{mazeBlue}
    , .neighbors{ 
        L::dfltUp // Up to mazeGrail
        , L::dfltRight // Right to mazeU
        , L::dfltDown // Down to mazeBase
        , L::dfltRight // Left to mazeU
    }
};

const Layout_t Layouts::mazeHumps {
    .tiles{
        "WWWWWWWWWWWWWWWWWWWW",
        "OOOOOOOOOOOOOOOOOOOO",
        "OOOOOOOOOOOOOOOOOOOO",
        "WWWWWOWWWWWWWWOWWWWW",
        "WWWWWOWWWWWWWWOWWWWW",
        "WWOOOOOOOWWOOOOOOOWW",
        "WWOOOOOOOWWOOOOOOOWW",
        "WWOWWWWWOWWOWWWWWOWW",
        "WWOWWWWWOWWOWWWWWOWW",
        "OOOWOOOWOWWOWOOOWOOO",
        "OOOWOOOWOWWOWOOOWOOO",
        "WWWWOWOWOWWOWOWOWWWW"
    }
    , .gcOffset{-3, 1}
    , .color{mazeBlue}
    , .neighbors{ 
        L::dfltUp // Up to nothing
        , L::dfltRight // Right to mazeBase
        , VI{2, -1} // Down to mazeU
        , VI{1, -2} // Left to mazeGrail
    }
};

const Layout_t Layouts::mazeBase {
    .tiles{
        "WWWWOWOWOOOOWOWOWWWW",
        "OOOWOOOWOOOOWOOOWOOO",
        "OOOWOOOWOOOOWOOOWOOO",
        "WWOWWWWWOOOOWWWWWOWW",
        "WWOWWWWWOOOOWWWWWOWW",
        "WWOOOOOOOOOOOOOOOOWW",
        "WWOOOOOOOOOOOOOOOOWW",
        "WWWWWOOOOOOOOOOWWWWW",
        "WWWWWOOOOOOOOOOWWWWW",
        "OOOOWOOOOOOOOOOWOOOO",
        "OOOOWOOOOOOOOOOWOOOO",
        "WWWWWWWWWWWWWWWWWWWW"
    }
    , .gcOffset{-2, 1}
    , .color{mazeBlue}
    , .neighbors{ 
        L::dfltUp // Up to mazeSwirls
        , VI{0, -2} // Right to mazeGrail
        , VI{2, -1} // Down to nothing
        , L::dfltLeft // Left to mazeHumps
    }
};
        
const Layout_t Layouts::mazeGrail {
    .tiles{
        "WWWWWWWWOOOOWWWWWWWW",
        "OOOOWOOWOOOOWOOWOOOO",
        "OOOOWOOWOOOOWOOWOOOO",
        "WWOOWOOWWOOWWOOWOOWW",
        "WWOOWOOWWOOWWOOWOOWW",
        "WWOOWOOOOOOOOOOWOOWW",
        "WWOOWOOOOOOOOOOWOOWW",
        "WWWWWWWWWOOWWWWWWWWW",
        "WWWWWWWWWOOWWWWWWWWW",
        "OOOWOOOOWOOWOOOOWOOO",
        "OOOWOOOOWOOWOOOOWOOO",
        "WWOWOWWWWOOWWWWOWOWW"
    }
    , .gcOffset{-2, -1}
    , .color{mazeBlue}
    , .neighbors{ 
        L::dfltUp // Up to black castle
        , VI{-1, 2} // Right to mazeHumps
        , L::dfltDown // Down to mazeSwirls
        , VI{0, 2} // Left to mazeBase
    }
};

const Layout_t Layouts::blackCastle {
    .tiles{
        "WWWWWOOOOOOOOOOWWWWW",
        "WOOOOWOOOOOOOOWOOOOW",
        "WOOOOWOOOOOOOOWOOOOW",
        "WOOOOWOOOOOOOOWOOOOW",
        "WOOOOWOOOOOOOOWOOOOW",
        "WOOOOOWOOOOOOWOOOOOW",
        "WOOOOOWWWWWWWWOOOOOW",
        "WOOOOOWWWOOWWWOOOOOW",
        "WOOOOOWWWOOWWWOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WWWWWWWWOOOOWWWWWWWW"
    }
    , .gcOffset{-2, -2}
    , .color{0, 0, 0, 255}
};

const Layout_t Layouts::blackCastleInsideBot {
    .tiles{
        "WWWWWWWWOOOOWWWWWWWW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WWWWWWWWOOOOWWWWWWWW"
    }
    , .gcOffset{-2, -3}
    , .color{198, 108, 58, 255}
};

const Layout_t Layouts::blackCastleInsideTop {
    .tiles{
        "WWWWWWWWWWWWWWWWWWWW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WOOOOOOOOOOOOOOOOOOW",
        "WWWWWWWWOOOOWWWWWWWW"
    }
    , .gcOffset{-2, -4}
    , .color{146, 70, 192, 255}
};


