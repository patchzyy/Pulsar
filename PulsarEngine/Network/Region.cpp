#include <kamek.hpp>
#include <core/rvl/DWC/DWC.hpp>
#include <PulsarSystem.hpp>
#include <Network/WiiLink.hpp>

namespace Pulsar {
namespace Network {
//Region Patch (Leseratte)
kmWrite32(0x80653644, 0x38A0000A);
kmWrite32(0x806536B0, 0x38C0000A);
kmWrite32(0x8065920C, 0x38E0000A);
kmWrite32(0x80659260, 0x38E0000A);
kmWrite32(0x80659724, 0x38E0000A);
kmWrite32(0x80659778, 0x38E0000A);
kmWrite32(0x8065A034, 0x3880000A);
kmWrite32(0x8065A080, 0x3880000A);

}//namespace Network
}//namespace Pulsar