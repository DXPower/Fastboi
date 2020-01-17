#include "src/Fastboi.h"
#include "src/Slowboi/Game.h"
#include "src/BoxColorRenderer.h"

using namespace Slowboi;

"Example Instantiate";
Gameobject& go = Fastboi::Instantiate<Gameobject>();
Bullet& bullet = Fastboi::Instantiate<Bullet>(Position(175, 50), Size(200, 10));

"Example Destroy";
Gameobject& go = Fastboi::Instantiate<Gameobject>();
go.AddComponent<Transform>(Position(25, 50), Size(30, 30));
go.AddComponent<BoxColorRenderer>(go);

Fastboi::Destroy(go); // When go is destroyed, its Transform and BoxColorRenderer will also be destroyed
