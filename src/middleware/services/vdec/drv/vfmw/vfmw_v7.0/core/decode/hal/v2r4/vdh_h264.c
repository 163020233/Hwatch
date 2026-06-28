/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2006-2024. All rights reserved.
 * Description: vdh h264
 * Author: AuthorNameMagicTag
 * Create: 2006-01-01
 */

#include "vdh_h264.h"
#include "dec_hal.h"
#include "dec_chn.h"
#include "dec_dev.h"
#include "dec_ref.h"
#include "dbg.h"
#include "vfmw_define.h"

static td_u32 g_slice_type_for_pmv[3] = {1, 2, 0};
// dec_pic->structure idx and mb_adptive_frame_field_flag
static td_u32 g_struct_trans[4][2]  = {{0, 3}, {1, 1}, {2, 2}, {3, 3}};
// frame sturcture idx and mb_adptive_frame_field_flag
static td_u32 g_list_struct_frame[4][2] = {{0, 1}, {3, 3}, {3, 3}, {2, 2}};
// frame sturcture idx and mb_adptive_frame_field_flag
static td_u32 g_list_struct_field[4][2] = {{0, 1}, {2, 2}, {2, 2}, {2, 2}};

static const td_u32 g_cabac_mn[160][8] = {
    {0x17211702, 0x15001240,  0x92b1d00,    0xb0137,   0x450029,   0x3f003f,   0x3f0000,        0x0},
    {0x14f10236,  0x34ae47f, 0xe968fa35, 0xff360733, 0x1f151f1f, 0x19320000,        0x0,        0x0},
    { 0x539f34e, 0xf541013e,  0x1090031, 0xdb760d29,  0x33ef753,  0x4560061, 0xf9480000,        0x0},
    { 0x143f34e, 0xf541013e, 0x1a43105a,  0x968d27f, 0xec680000,        0x0,        0x0,        0x0},
    { 0xc31fc49, 0x1132fa56, 0xef5ffa3d,  0x92df943, 0xfb4afc4a, 0xfb50f948,  0x13a0000,        0x0},
    {0xfd45fa51, 0xf5600637,  0x743fb56,  0x258003a, 0xfd4cf65e,  0x5360445, 0xfd510058,        0x0},
    {0x16192200, 0x10001a22, 0x13162800,    0xb0137,   0x450029,   0x3f003f,   0x3f0000,        0x0},
    {0x14f10236,  0x34ae47f, 0xe968fa35, 0xff360733, 0x1f151f1f, 0x19320000,        0x0,        0x0},
    { 0x241fa47, 0xf34f0534, 0xfe090429, 0xe3760d29,  0x33ef753,  0x4560061, 0xf9480000,        0x0},
    {0xfc4cfa47, 0xf34f0534, 0x39022924, 0x1a45d37f, 0xf1650000,        0x0,        0x0,        0x0},
    { 0x932fd46,  0xa360645, 0xf35a0034,  0x82bff42, 0xff4d0146, 0xfe56fb48,   0x3d0000,        0x0},
    {0xfe45fb52, 0xf660023b,  0x24bfd57, 0xfd640138, 0xfd4afa55,   0x3bfd51, 0xf956fb5f,        0x0},
    {0x1d101900,  0xe001428, 0x140a1d00,    0xb0137,   0x450029,   0x3f003f,   0x3f0000,        0x0},
    {0x14f10236,  0x34ae47f, 0xe968fa35, 0xff360733, 0x1f151f1f, 0x19320000,        0x0,        0x0},
    {0x1a10fc55, 0xe8660539, 0xf633fd3e, 0xe5630d29,  0x33ef753,  0x4560061, 0xf9480000,        0x0},
    {0xfe4afc55, 0xe8660539, 0x3600252a,  0xc61e07f, 0xea750000,        0x0,        0x0,        0x0},
    { 0x639ef49,  0xe39fa5d, 0xf258fa2c,  0x4370337, 0xfc4ffe4b, 0xf461f932,  0x13c0000,        0x0},
    {0xf559f167, 0xeb741339, 0x143a0454,  0x660013f, 0xfb55f36a,  0x53f064b, 0xfd5aff65,        0x0},
    {0x17211702, 0x15001240,  0x92b1d00,   0x2dfc4e, 0xfd600029,   0x3f003f,   0x3f0000,        0x0},
    {0x14f10236,  0x34ae47f, 0xe968fa35, 0xff360733,  0xc280b33,  0xe3b0000,        0x0,        0x0},
    { 0x539f34e, 0xf541013e,  0x1090031, 0xdb760d29,  0x33ef753,  0x4560061, 0xf9480000,        0x0},
    { 0x143f34e, 0xf541013e, 0x1a43105a,  0x968d27f, 0xec680000,        0x0,        0x0,        0x0},
    { 0xc31fc49, 0x1132fa56, 0xef5ffa3d,  0x92df943, 0xfb4afc4a, 0xfb50f948,  0x13a0000,        0x0},
    {0xfd45fa51, 0xf5600637,  0x743fb56,  0x258003a, 0xfd4cf65e,  0x5360445, 0xfd510058,        0x0},
    {0x16192200, 0x10001a22, 0x13162800,  0xd0f0733,  0x2500029,   0x3f003f,   0x3f0000,        0x0},
    {0x14f10236,  0x34ae47f, 0xe968fa35, 0xff360733, 0x19201531, 0x15360000,        0x0,        0x0},
    { 0x241fa47, 0xf34f0534, 0xfe090429, 0xe3760d29,  0x33ef753,  0x4560061, 0xf9480000,        0x0},
    {0xfc4cfa47, 0xf34f0534, 0x39022924, 0x1a45d37f, 0xf1650000,        0x0,        0x0,        0x0},
    { 0x932fd46,  0xa360645, 0xf35a0034,  0x82bff42, 0xff4d0146, 0xfe56fb48,   0x3d0000,        0x0},
    {0xfe45fb52, 0xf660023b,  0x24bfd57, 0xfd640138, 0xfd4afa55,   0x3bfd51, 0xf956fb5f,        0x0},
    {0x1d101900,  0xe001428, 0x140a1d00,  0x722f758, 0xec7f0029,   0x3f003f,   0x3f0000,        0x0},
    {0x14f10236,  0x34ae47f, 0xe968fa35, 0xff360733, 0x15211332, 0x113d0000,        0x0,        0x0},
    {0x1a10fc55, 0xe8660539, 0xf633fd3e, 0xe5630d29,  0x33ef753,  0x4560061, 0xf9480000,        0x0},
    {0xfe4afc55, 0xe8660539, 0x3600252a,  0xc61e07f, 0xea750000,        0x0,        0x0,        0x0},
    { 0x639ef49,  0xe39fa5d, 0xf258fa2c,  0x4370337, 0xfc4ffe4b, 0xf461f932,  0x13c0000,        0x0},
    {0xf559f167, 0xeb741339, 0x143a0454,  0x660013f, 0xfb55f36a,  0x53f064b, 0xfd5aff65,        0x0},
    {0xef7ff366,   0x52f94a, 0xeb6be57f, 0xe17fe87f, 0xee5fe57f, 0xeb72e27f,        0x0,        0x0},
    {0xef7bf473, 0xf07af573, 0xfd47fa2a, 0xfb32fd36, 0xfe3e003a,  0x13ffe48, 0xff4af75b,        0x0},
    {0xf95df557, 0xfd4dfb47, 0xfc3ffc44, 0xf454f93e, 0xf941083d,  0x538fe42,  0x140003d, 0xfe4e0000},
    {0x18000f09,  0x8190d12,  0xf090d13,  0xa250c12,  0x61d1421,  0xf1e042d,  0x13a003e,  0x73d0000},
    {0xfa5dfa54, 0xf84f0042, 0xff47003e, 0xfe3cfe3b, 0xfb4bfd3e, 0xfc3af742, 0xff4f0047,  0x3440000},
    { 0xf060613,  0x7100c0e, 0x120d0d0b,  0xd0f0f10,  0xc170d17,  0xf140e1a,  0xe2c1128, 0x112f0000},
    {0xf43ffe44, 0xf154f368, 0xfb43fb1b, 0xfd27fe2c,   0x2ef040, 0xf844f64e, 0xfa4df656,        0x0},
    { 0x1320734,  0xa23002c,  0xb26012d,   0x2e052c, 0x1f110133,  0x7321c13, 0x10210e3e,        0x0},
    { 0xc260b2d,  0xf270b2a,  0xd2c102d,  0xc290a31, 0x1e22122a,  0xa371133, 0x112e0059,        0x0},
    { 0xa2cf93e,  0xf240e28, 0x101b0c1d,  0x12c1424, 0x1220052a,  0x1300a3e, 0x112e0940,        0x0},
    {0x18111515, 0x19161f1b, 0x161d1323,  0xe320a39,  0x73ffe4d, 0xfc52fd5e,  0x945f46d,        0x0},
    {0xfd46f85d, 0xf65ae27f, 0xf45cf137, 0xf63cfa3e, 0xfc41f449, 0xf84cf950, 0xf758ef6e,        0x0},
    {0xf36cf164, 0xf365f35b, 0xf45ef658, 0xf054f656, 0xf953f357, 0xed5e0146,   0x48fb4a, 0x123b0000},
    {0x1aed16ef, 0x1aef1ee7, 0x1cec21e9, 0x25e521e9, 0x28e426ef, 0x21f528f1, 0x29fa2601, 0x29110000},
    {0xf468f561, 0xf060f958, 0xf855f955, 0xf755f358,  0x442fd4d, 0xfd4cfa4c,  0xa3aff4c, 0xff530000},
    {0x24dd24de, 0x20e625e2, 0x2ce022ee, 0x22f128f1, 0x21f923fb, 0x21002602, 0x210d1723,  0xd3a0000},
    {0xff4afa61, 0xf95bec7f, 0xf561ec54, 0xf54ffa49, 0xfc4af356, 0xf360f561, 0xed750000,        0x0},
    {0xf866f164,   0x5f0000,        0x0,        0x0,        0x0,        0x0,        0x0,        0x0},
    {0x1efa1b03, 0x1a160000,        0x0,        0x0,        0x0,        0x0,        0x0,        0x0},
    {0xf963f25f,  0x25f0000,        0x0,        0x0,        0x0,        0x0,        0x0,        0x0},
    {0x1dfd1a00, 0x161e0000,        0x0,        0x0,        0x0,        0x0,        0x0,        0x0},
    {0xfc38fb52, 0xf94cea7d, 0xf84efb21, 0xfc30fe35, 0xfd3ef347, 0xf64ff456, 0xf35af261,        0x0},
    {0xfc4b0248, 0xf54bfd47,  0xf2ef345,   0x3e0041, 0x1525f148,  0x9391036,   0x3e0c48,        0x0},
    {0x25f023fc, 0x26f826fd, 0x25032605, 0x2a002310, 0x27160e30, 0x1b25153c,  0xc440261,        0x0},
    {  0x4cfb4a,   0x46f54b,  0x1440041, 0xf249033e,  0x43eff44, 0xf34b0b37,  0x5400c46,        0x0},
    {0x1ff923f1, 0x22fd2203, 0x24ff2205, 0x200b2305, 0x220c270b, 0x1e1d221a, 0x1d271342,        0x0},
    {       0x0,        0x0, 0xfd4bff17,  0x122012b,   0x36fe37,   0x3d0140,   0x44f75c,        0x0},
    {0xef78ec70, 0xee72f555, 0xf15cf259, 0xe647f151, 0xf2500044, 0xf246e838, 0xe944e832, 0xf54a0000},
    {0x17f31af3, 0x28f131f2, 0x2c032d06, 0x2c222136, 0x13520000,        0x0,        0x0,        0x0},
    {0xf26af361, 0xf15af45a, 0xee58f649, 0xf74ff256, 0xf649f646, 0xf645fb42, 0xf740fb3a,  0x23b0000},
    {0x15f618f5, 0x1cf81cff, 0x1d031d09, 0x23141d24,  0xe430000,        0x0,        0x0,        0x0},
    {0xe57ee462, 0xe765e943, 0xe452ec5e, 0xf053ea6e, 0xeb5bee66, 0xf35de37f,        0x0,        0x0},
    {0xf95cfb59, 0xf960f36c, 0xfa4cfe2c,   0x2d0034, 0xfd40fe3b, 0xfc46fc4b, 0xf852ef66,        0x0},
    {0xfe55fa4e, 0xff4bf94d,  0x2360532, 0xfd440132,  0x62afc51,  0x13ffc46,   0x430239, 0xfe4c0000},
    { 0xb1c0228,  0x32c0031,   0x2e022c,  0x233002f,  0x427023e,  0x62e0036,  0x336023a,  0x43f0000},
    {0xf36af06a, 0xf657eb72, 0xee6ef262, 0xea6eeb6a, 0xee67eb6b, 0xe96ce670, 0xf660f45f, 0xfb5b0000},
    { 0xe0b0b0e,  0x90b120b, 0x150917fe, 0x20f120f1, 0x22eb27e9, 0x2adf29e1, 0x2ee426f4, 0x151d0000},
    {0xfd2eff41, 0xff39f75d, 0xf74d0318,   0x2a0030,   0x37fa3b, 0xf947f453, 0xf557e277,        0x0},
    { 0xb230440,  0x13d0b23, 0x12190c18,  0xd1d0d24, 0xf65df949, 0xfe490d2e,  0x931f964,        0x0},
    { 0x6330639,  0x7350634,  0x6370b2d,  0xe240835, 0xff520737, 0xfd4e0f2e, 0x161fff54,        0x0},
    {0xf75dea5e, 0xfb560943, 0xfc50f655, 0xff46073c,  0x93a053d,  0xc320f32, 0x12311136,        0x0},
    {0x2de835d3, 0x30e641d5, 0x2bed27f6, 0x1e09121a, 0x141b0039, 0xf252fb4b, 0xed61dd7d,        0x0},
    {0xfd4af75c, 0xf857e97e,  0x13afd1d, 0xff240126,  0x22bfa37,   0x3a0040, 0xfd4af65a,        0x0},
    { 0x9350235,  0x535fe3d,   0x380038, 0xf33ffb3c, 0xff3e0439, 0xfa450439,  0xe270433,  0xd440000},
    {0x19071ef9, 0x1c031c04, 0x200022ff, 0x1e061e06, 0x20091f13, 0x1a1b1a1e, 0x25141c22, 0x11460000},
    { 0xa29072e, 0xff330731,  0x8340929,  0x62f0237,  0xd290a2c,  0x6320535,  0xd31043f,  0x6400000},
    {0x1b001c00, 0x1ffc1b06, 0x22081e0a, 0x18162113, 0x16201a1f, 0x15291a2c, 0x172f1041,  0xe470000},
    { 0x536063c,  0x63b0645,   0x46fc1d,  0x51f072a,  0x13bfe3a, 0xfd48fd51, 0xf5610000,        0x0},
    { 0x340013d,  0x93f0000,        0x0,        0x0,        0x0,        0x0,        0x0,        0x0},
    { 0x143053b,  0x9430000,        0x0,        0x0,        0x0,        0x0,        0x0,        0x0},
    {0xfe45fe3b,  0x6460000,        0x0,        0x0,        0x0,        0x0,        0x0,        0x0},
    { 0x83c063f, 0x11410000,        0x0,        0x0,        0x0,        0x0,        0x0,        0x0},
    {0xff300044, 0xfc45f858,   0x3a0805,  0xa0e0e12,  0xd1b0228,   0x3afd46, 0xfa4ff855,        0x0},
    { 0x7321027,  0x52c0434,  0xb30fb3c, 0xff3b003b, 0x1621052c,  0xe2bff4e,   0x3c0945,        0x0},
    {0x101e1220, 0x1223161d, 0x181f1726, 0x122b1429,  0xb3f093b,  0x940ff5e, 0xfe59f76c,        0x0},
    { 0xa2c091f,  0xc2b0335,  0xe220a26, 0xfd340d28, 0x1120072c,  0x7260d32,  0xa391a2b,        0x0},
    {0x15181714, 0x1a171b20, 0x1c171c18, 0x17281820, 0x1c1d172a, 0x13391635, 0x163d0b56,        0x0},
    {       0x0,        0x0, 0xfa42f923, 0xf92af82d, 0xfb30f438, 0xfa3cfb3e, 0xf842f84c,        0x0},
    {0xfc4ff947, 0xfb45f746, 0xf842f644, 0xed49f445, 0xf046f143, 0xec3eed46, 0xf042ea41, 0xec3f0000},
    { 0x9fe1af7, 0x21f727f9, 0x29fe2d03, 0x31092d1b, 0x243b0000,        0x0,        0x0,        0x0},
    {0xfb55fa51, 0xf64df951, 0xef50ee49, 0xfc4af653, 0xf747f743, 0xff3df842, 0xf242003b,  0x23b0000},
    {0x15f321f2, 0x27f92efe, 0x33023c06, 0x3d113722, 0x2a3e0000,        0x0,        0x0,        0x0},
    {0xd97fee5b, 0xef60e651, 0xdd62e866, 0xe961e577, 0xe863eb6e, 0xee66dc7f,        0x0,        0x0},
    {  0x50fb59, 0xf95efc5c, 0xe970f147, 0xf93d0035, 0xfb42f54d, 0xf750f754, 0xf657de7f,        0x0},
    {0xf367f35b, 0xf759f25c, 0xf84cf457, 0xe96ee869, 0xf64eec70, 0xef63b27f, 0xba7fce7f, 0xd27f0000},
    { 0x42d0a1c,  0xa1f21f5, 0x34d5120f, 0x1c0023ea, 0x26e72200, 0x27ee20f4, 0x66a20000, 0x38f10000},
    {0xeb7ee97c, 0xec6ee67e, 0xe77cef69, 0xe579e575, 0xef66e675, 0xe574df7a, 0xf65ff264, 0xf85f0000},
    {0x13fa12fa,  0xe001af4, 0x1ff021e7, 0x21ea25e4, 0x27e22ae2, 0x2fd62ddc, 0x31de29ef, 0x20090000},
    {  0x270041, 0xf154dd7f, 0xeb65fd27, 0xfb35f93d, 0xf54bf14d, 0xef5be76b, 0xe76fe47a,        0x0},
    {0xfc42fb4e, 0xfc47f848,  0x23bff37, 0xf946fa4b, 0xf859de77, 0xfd4b2014, 0x1e16d47f,        0x0},
    {0x21fc1d0a, 0x25fb33e3, 0x27f734de, 0x45c643c1, 0x2cfb2007, 0x37e32001,     0x1b24,        0x0},
    {0xef6fe472, 0xfa59fe50, 0xfc52f755, 0xf851ff48,  0x5400143,  0x9380045,  0x1450745,        0x0},
    {0x45b93fc1, 0x42c04db6, 0x36d934dd, 0x29f62400, 0x28ff1e0e, 0x1c1a1725,  0xc370b41,        0x0},
    {0xfe49f468, 0xf75be17f, 0xf54cf62c, 0xf634f639, 0xf73af048, 0xf945fc45, 0xfb4af756,        0x0},
    {  0x36fb3d,   0x3aff3c, 0xfd3df843, 0xe754f24a, 0xfb410534,  0x239003d, 0xf745f546, 0x12370000},
    {0x21e722e2, 0x24e426e4, 0x26e522ee, 0x23f022f2, 0x20f825fa, 0x23001e0a, 0x1c121a19, 0x1d290000},
    {0xf945fa43, 0xf04dfe40,  0x23dfa43, 0xfd400239, 0xfd41fd42,   0x3e0933, 0xff42fe47, 0xfe4b0000},
    {0x25df27dc, 0x28db26e2, 0x2edf2ae2, 0x28e831e3, 0x26f428f6, 0x26fd2efb, 0x1f141d1e, 0x192c0000},
    { 0x3370738,  0x737083d,  0x242f722,  0x1200b1f,  0x534fe37, 0xfe430049, 0xf8590000,        0x0},
    {0xfc47003a,  0x73d0000,        0x0,        0x0,        0x0,        0x0,        0x0,        0x0},
    {  0x4b0248,  0x84d0000,        0x0,        0x0,        0x0,        0x0,        0x0,        0x0},
    {0xff46f748,  0xe3c0000,        0x0,        0x0,        0x0,        0x0,        0x0,        0x0},
    { 0xc300b31, 0x1a2d0000,        0x0,        0x0,        0x0,        0x0,        0x0,        0x0},
    {0xfd350044, 0xf94af758,  0x3340704,  0xa081108, 0x10130325, 0xff3dfb49, 0xff46fc4e,        0x0},
    { 0x9291219,  0x920052b,  0x92f002c,   0x33022e, 0x1326fc42,  0xf260c2a,  0x9220059,        0x0},
    { 0xe23121f, 0x1123151e, 0x112d142a, 0x122d1b1a, 0x10360742, 0x10380b49,  0xa43f674,        0x0},
    {0x1025002f, 0x12230b25,  0xc290a29,  0x2300c29,  0xd29003b,  0x3321328,  0x3421232,        0x0},
    {0x16161716, 0x1b152114, 0x1a1c1e18, 0x1b22122a, 0x19271232,  0xc461536,  0xe470b53,        0x0},
    {       0x0,        0x0, 0xfb470018, 0xff24fe2a, 0xfe34f739, 0xfa3ffc41, 0xfc43f952,        0x0},
    {0xfb55fa51, 0xf64df951, 0xef50ee49, 0xfc4af653, 0xf747f743, 0xff3df842, 0xf242003b,  0x23b0000},
    {0x11f620f3, 0x2af731fb, 0x35004003, 0x440a421b, 0x2f390000,        0x0,        0x0,        0x0},
    {0xfd51fd4c, 0xf948fa4e, 0xf448f244, 0xfd46fa4c, 0xfb42fb3e,   0x39fc3d, 0xf73c0136,  0x23a0000},
    {0x11f620f3, 0x2af731fb, 0x35004003, 0x440a421b, 0x2f390000,        0x0,        0x0,        0x0},
    {0xdc7fef5b, 0xf25fe754, 0xe756f459, 0xef5be17f, 0xf24cee67, 0xf35adb7f,        0x0,        0x0},
    { 0xb50054c,  0x254054e, 0xe873ea52, 0xf73e0035,   0x3bf255, 0xf359f35e, 0xf55ce37f,        0x0},
    {0xfc56f458, 0xfb52fd48, 0xfc43f848, 0xf059f745, 0xff3b0542,  0x439fc47, 0xfe47023a, 0xff4a0000},
    { 0x427002a,  0x7220b1d,  0x81f0625,  0x72a0328,  0x8210d2b,  0xd24042f,  0x337023a,  0x63c0000},
    {0xea7fe77f, 0xe778e57f, 0xed72e975, 0xe776e675, 0xe871e476, 0xe178db7c, 0xf65ef166, 0xf6630000},
    {0x11f310f7, 0x11f41beb, 0x25e229d8, 0x2ad730d1, 0x27e02ed8, 0x34cd2ed7, 0x34d92bed, 0x200b0000},
    {0xfa37043d, 0xf253db7f, 0xeb64f239, 0xf443f547, 0xf64deb55, 0xf058e968, 0xf162db7f,        0x0},
    {0xfc2cff45,   0x3ef933, 0xfc2ffa2a, 0xfd29fa35,  0x84cf74e, 0xf5530934,   0x43fb5a,        0x0},
    { 0x82c0b2c,  0xe2a0730,  0x4380434,  0xd250931, 0x133a0a30,  0xc2d0045, 0x1421083f,        0x0},
    {0xf36ace7f, 0xfb5c1139, 0xfb56f35e, 0xf45bfe4d,   0x47ff49,  0x440f951,  0x5400f39,        0x0},
    {0x3dc938d2, 0x3ece51bd, 0x2dec23fe, 0x1c0f2201, 0x27011e11, 0x1426122d,  0xf36004f,        0x0},
    {0xfb4ff568, 0xf55be27f, 0xf652f830, 0xf83df842, 0xf946f24b, 0xf64ff753, 0xf45cee6c,        0x0},
    { 0x143f148, 0xfb4bf850, 0xeb53eb40, 0xf31fe740, 0xe35e094b, 0x113ff84a, 0xfb23fe1b,  0xd5b0000},
    {0x23ee21e7, 0x1cfd180a, 0x1b0022f2, 0x34d427e8, 0x13111f19, 0x241d1821, 0x220f1e14, 0x16490000},
    { 0x1430044, 0xf6430144,   0x4d0240,   0x44fb4e,  0x737053b,  0x2410e36,  0xf2c053c,  0x2460000},
    {0x24f025f2, 0x25ef2001, 0x220f1d0f, 0x18192216, 0x1f102312, 0x1f1c2129, 0x241c1b2f, 0x153e0000},
    {  0x41fe4f,   0x48fc5c, 0xfc4fea45, 0xf04bfe3a,  0x13af34e, 0xf753fc51, 0xf3630000,        0x0},
    { 0x341f945,  0x84d0000,        0x0,        0x0,        0x0,        0x0,        0x0,        0x0},
    {0x1422131f, 0x1b2c0000,        0x0,        0x0,        0x0,        0x0,        0x0,        0x0},
    {0xfe4cee56,  0xc460000,        0x0,        0x0,        0x0,        0x0,        0x0,        0x0},
    {0x121f131a, 0x24180000,        0x0,        0x0,        0x0,        0x0,        0x0,        0x0},
    {0xfa380344, 0xf847f362, 0xf351fa26, 0xf33efa3a, 0xfe3bf049, 0xf64cf356, 0xf753f657,        0x0},
    {0xf642033e, 0xfd44ec51,   0x1e0107, 0xfd17eb4a, 0x1042e97c, 0x11252cee, 0x32deea7f,        0x0},
    {0x13100f24,  0xf24151c, 0x19151e14, 0x1f0c1b10, 0x182a005d,  0xe380f39, 0x1a26e87f,        0x0},
    { 0x540f446,  0xb370538,   0x450241, 0xfa4a0536,  0x736fa4c, 0xf552fe4d, 0xfe4d192a,        0x0},
    {0x18171b10, 0x181e1f1d, 0x1629162a, 0x103c0f34,  0xe3c034e, 0xf07b1535, 0x1638193d,        0x0},
    {       0x0,        0x0, 0xf747f925, 0xf82cf531, 0xf638f43b, 0xf83ff743, 0xfa44f64f,        0x0},
    {0xfd4ef84a, 0xf748f648, 0xee4bf447, 0xf53ffb46, 0xef4bf248, 0xf043f835, 0xf23bf734, 0xf5440000},
    { 0x9fe1ef6, 0x1ffc21ff, 0x21071f0c, 0x25171f26, 0x14400000,        0x0,        0x0,        0x0},
    {0xfd4ef84a, 0xf748f648, 0xee4bf447, 0xf53ffb46, 0xef4bf248, 0xf043f835, 0xf23bf734, 0xf5440000},
    { 0x9fe1ef6, 0x1ffc21ff, 0x21071f0c, 0x25171f26, 0x14400000,        0x0,        0x0,        0x0},
};

static td_s32 hal_h264_write_cabac_table(td_u8 *mn_vir_addr)
{
    vfmw_check_sec_func(memcpy_s(mn_vir_addr, H264_CA_MN_ADDR_LEN, g_cabac_mn, H264_CA_MN_ADDR_LEN));

    return DEC_OK;
}

static td_s32 hal_h264_split_slice_list(dec_pic_param_h264 *pic_param, dec_slice_param_h264 *slice_head,
    td_u16 slice_num, td_void *h264_slice_list)
{
    vfmw_unused(pic_param);
    dec_chan_slice_list *slice_list = (dec_chan_slice_list *)h264_slice_list;

    slice_list->fake_slice = slice_head->first_mb_in_slice == 0 ? 0 : 1;

    if (VDH_OLP_NUM > 1) {
        dprint(PRN_ERROR, "DEC only support one BD core!\n");
    }

    slice_list->list_head[0] = slice_head;
    slice_list->list_len[0]  = slice_num;
    slice_list->list_num = 1;

    return DEC_OK;
}

static td_void hal_h264_reorder_q_matrix(td_u32 *q_matrix, td_u32 size)
{
    td_s32 i, idx, idx_1;
    td_u32 qmatrix4x4[6][4];
    td_u32 qmatrix8x8[2][16];

    vfmw_unused(size);
    for (i = 0; i < 24; i++) { /* 24 :a number */
        qmatrix4x4[i / 4][i % 4] = (((q_matrix[i] >> 24) & 0xff) << 24) | /* 4 24 :a number */
                                   (((q_matrix[i] >> 8) & 0xff) << 16)   | /* 8 16 :a number */
                                   (((q_matrix[i] >> 16) & 0xff) << 8)   | /* 8 16 :a number */
                                   ((q_matrix[i]) & 0xff);
    }

    for (i = 0; i < 16; i++) { /* 16 :a number */
        idx = 24 + 2 * i; /* 24 2 :a number */
        idx_1 = idx + 1;
        qmatrix8x8[i / 8][(2 * i) % 16] = (((q_matrix[idx_1] >> 8) & 0xff) << 24) | /* 2 8 16 24 :a number */
                                          (((q_matrix[idx] >> 8) & 0xff) << 16) | /* 8 16  :a number */
                                          ((q_matrix[idx_1] & 0xff) << 8) | /* 8 :a number */
                                          ((q_matrix[idx]) & 0xff);

        qmatrix8x8[i / 8][(2 * i + 1) % 16] = (((q_matrix[idx_1] >> 24) & 0xff) << 24) | /* 2 8 16 24 :a number */
                                              (((q_matrix[idx] >> 24) & 0xff) << 16)  | /* 16 24 :a number */
                                              (((q_matrix[idx_1] >> 16) & 0xff) << 8)  | /* 8 16 :a number */
                                              ((q_matrix[idx] >> 16) & 0xff); /* 16 :a number */
    }

    for (i = 0; i < 24; i++) { /* 24 num */
        q_matrix[i] = qmatrix4x4[i / 4][i % 4]; /* 4 num */
    }

    for (i = 0; i < 32; i++) { /* 32 num */
        q_matrix[i + 24] = qmatrix8x8[i / 16][i % 16]; /* 16 24 num */
    }
}

static td_s32 hal_h264_write_q_matrix(td_u32 *q_matrix, td_u32 size, td_u32 *vir_addr)
{
    td_u32 i = 0;

    hal_h264_reorder_q_matrix(q_matrix, size);
    for (i = 0; i < size; i++) {
        *(vir_addr + i) = *(q_matrix + i);
    }

    return DEC_OK;
}

static td_s32 hal_h264_write_slice_msg_stream_info(const dec_pic_param_h264 *pic_param,
    dec_slice_param_h264 *slice_param, td_u32 *slot)
{
    UADDR  stream_base_addr, stream_start_addr;
    td_u32 d32, i;

    if (slice_param->valid_bitlen[0] == 0) {
        return DEC_ERR;
    }

    for (i = 0; i < 2; i++) { /* 2 num */
        if (slice_param->valid_bitlen[i] > 0) {
            stream_base_addr  = vfmw_align_dn(pic_param->stream_base_addr, 16); /* 16 num */
            stream_start_addr = vfmw_align_dn(slice_param->bytes_pos[i], 16); /* 16 num */

            d32 = (stream_start_addr - stream_base_addr);
            WR_MSGWORD(slot + 3 * i, d32); /* 3 num */

            d32 = (slice_param->bits_offset[i] + ((slice_param->bytes_pos[i] & 0xf) << 3)) & 0x7f; /* 3 num */
            WR_MSGWORD(slot + 3 * i + 1, d32); /* 3 num */

            d32 = slice_param->valid_bitlen[i];
            WR_MSGWORD(slot + 3 * i + 2, d32); /* 3 2 num */
        } else {
            WR_MSGWORD(slot + 3 * i, 0); /* 3 num */
            WR_MSGWORD(slot + 3 * i + 1, 0); /* 3 num */
            WR_MSGWORD(slot + 3 * i + 2, 0); /* 3 2 num */
        }
    }

    return DEC_OK;
}

static td_void hal_h64_write_slice_msg_mv_addr_info(const dec_pic_param_h264 *pic_param,
    dec_chan_mem_param *task_mem, dec_slice_param_h264 *slice_param, td_u32 *slot)
{
    h264_dec_list *dec_list = &(slice_param->declist[1][0]);
    td_u32 tmp0 = 0;
    td_u32 tmp1 = 0;

    if (g_slice_type_for_pmv[slice_param->slice_type] == 2) { /* 2 num */
        if (pic_param->pic_structure == 0) { // frame
            tmp0 = slice_param->apc_2_mv_addr;
            if (dec_list->frame_structure == 3) { /* 3 num */
                // field pair
                tmp1 = slice_param->apc_2_mv_addr + task_mem->half_pmvblk_offset;
            } else {
                // not field pair
                tmp1 = 0;
            }
        } else {  // field
            if (dec_list->frame_structure == 0 && dec_list->is_used == 3) { /* 3 num */
                // frame or mbaff frame
                tmp0 = slice_param->apc_2_mv_addr;
            } else if (dec_list->list_structure == 1) {
                // top field
                tmp0 = slice_param->apc_2_mv_addr;
            } else {
                // bottom field
                tmp0 = slice_param->apc_2_mv_addr + task_mem->half_pmvblk_offset;
            }

            tmp1 = 0;
        }
    } else {
        tmp0 = 0;
        tmp1 = 0;
    }

    WR_MSGWORD(slot + 8, (tmp0 & 0xFFFFFFF0)); /* 8 num */
    WR_MSGWORD(slot + 9, (tmp1 & 0xFFFFFFF0)); /* 9 num */
}

static void hal_h264_write_slice_msg_ref_pic_frame(const dec_pic_param_h264 *pic_param,
    dec_slice_param_h264 *slice_param, td_u32 *slot, td_u32 list_num)
{
    h264_dec_list *list = TD_NULL;
    td_u32 mbaff_flag = pic_param->mb_adaptive_frame_field_flag;
    td_u32 list_size = slice_param->list_x_size[list_num];
    td_u32 d32, dn, i;

    d32 = 0;
    dn  = 12 + list_num * 4; /* 12 4 num */
    for (i = 0; i < list_size; i++) {
        list = &slice_param->declist[list_num][i];
        d32 |= ((list->frame_is_long_term << 3) | /* 3 num */
                (g_list_struct_frame[list->frame_structure][mbaff_flag] & 0x3)) << 4 * (i % 8); /* 4 8 num */

        if ((i % 8) == 7 || i == list_size - 1) { /* 7 8 num */
            WR_MSGWORD(slot + dn, d32);
            d32 = 0;
            dn++;
        }
    }

    d32 = 0;
    dn  = 20 + list_num * 8; /* 20 8 num */
    for (i = 0; i < list_size; i++) {
        d32 |= ((slice_param->idx_2_apc[list_num][i] << 1) & 0x1f) << 5 * (i % 4); /* 5 4 num */

        if ((i % 4) == 3 || i == list_size - 1) { /* 4 3 num */
            WR_MSGWORD(slot + dn, d32);
            d32 = 0;
            dn++;
        }
    }
}

static void hal_h264_write_slice_msg_ref_pic_field(const dec_pic_param_h264 *pic_param,
    dec_slice_param_h264 *slice_param, td_u32 *slot, td_u32 list_num)
{
    h264_dec_list *list = TD_NULL;
    td_u32 mbaff_flag = pic_param->mb_adaptive_frame_field_flag;
    td_u32 structure = 0;
    td_u32 dat0 = 0;
    td_u32 dat1 = 0;
    td_u32 dn0  = 12 + list_num * 4; /* 12 4 num */
    td_u32 dn1  = 20 + list_num * 8; /* 20 8 num */
    td_u32 i;

    for (i = 0; i < slice_param->list_x_size[list_num]; i++) {
        list = &slice_param->declist[list_num][i];

        if (list->list_structure == 1) {
            // top field
            structure = list->is_used != 3 ? list->top_structure : list->frame_structure; /* 3 num */
            structure = g_list_struct_field[structure][mbaff_flag];

            dat0 |= ((list->top_is_long_term << 3) | ((td_u32)(list->top_structure == 1) << 2) | /* 3 2 num */
                (structure & 0x3)) << 4 * (i % 8); /* 4 8 num */

            dat1 |= ((slice_param->idx_2_apc[list_num][i] << 1) & 0x1f) << 5 * (i % 4); /* 5 4 num */
        } else if (list->list_structure == 2) { /* 2 num */
            // bottom field
            structure = list->is_used != 3 ? list->btm_structure : list->frame_structure; /* 3 num */
            structure = g_list_struct_field[structure][mbaff_flag];

            dat0 |= ((list->btm_is_long_term << 3) | ((td_u32)(list->btm_structure == 1) << 2) | /* 3 2 num */
                (structure & 0x3)) << 4 * (i % 8); /* 4 8 num */

            dat1 |= (((slice_param->idx_2_apc[list_num][i] << 1) | 1) & 0x1f) << 5 * (i % 4); /* 5 4 num */
        }

        if ((i % 8) == 7 || i == slice_param->list_x_size[list_num] - 1) { /* 8 7 num */
            WR_MSGWORD(slot + dn0, dat0);
            dat0 = 0;
            dn0++;
        }

        if ((i % 4) == 3 || i == slice_param->list_x_size[list_num] - 1) { /* 4 3 num */
            WR_MSGWORD(slot + dn1, dat1);
            dat1 = 0;
            dn1++;
        }
    }
}

static td_void hal_h264_write_slice_msg_ref_pic_info(dec_pic_param_h264 *pic_param,
    dec_slice_param_h264 *slice_param, td_u32 *slot)
{
    if (pic_param->pic_structure == 0) { // frame reference
        hal_h264_write_slice_msg_ref_pic_frame(pic_param, slice_param, slot, 0);

        if (g_slice_type_for_pmv[slice_param->slice_type] == 2) { /* 2 num */
            hal_h264_write_slice_msg_ref_pic_frame(pic_param, slice_param, slot, 1);
        }
    } else { // field reference
        hal_h264_write_slice_msg_ref_pic_field(pic_param, slice_param, slot, 0);

        if (g_slice_type_for_pmv[slice_param->slice_type] == 2) { // 2 B Slice
            hal_h264_write_slice_msg_ref_pic_field(pic_param, slice_param, slot, 1);
        }
    }
}

static td_void hal_h264_write_slice_msg_apc_info(const dec_pic_param_h264 *pic_param,
    const dec_slice_param_h264 *slice_param, td_u32 *slot)
{
    td_u32 d32 = 0, i = 0;
    td_u32 dn_num = 36;

    vfmw_unused(pic_param);

    for (i = 0; i < 32; i++) { /* 32 num */
        d32 |= ((slice_param->apc_2_ref_idx[i] & 0x1f) << 5 * (i % 4)); /* 5 4 num */

        if ((i % 4) == 3) { /* 3 4 num */
            WR_MSGWORD(slot + dn_num, d32);
            dn_num++;
            d32 = 0;
        }
    }
}

static td_void hal_h264_write_slice_msg_wp_info(const dec_pic_param_h264 *pic_param,
    const dec_slice_param_h264 *slice_param, td_u32 *slot)
{
    td_u32 *slot0, *slot1, *slot2;
    td_u32  i = 0, d32 = 0;

    if ((pic_param->weighted_pred_flag  == 1 && g_slice_type_for_pmv[slice_param->slice_type] == 1) ||
        (pic_param->weighted_bipred_idc == 1 && g_slice_type_for_pmv[slice_param->slice_type] == 2)) { /* 2 num */
        // list0
        // D0~D31 of burst1, dn_num = 1*64 + D0 ,luma wp
        slot0 = slot + 64;

        // D0~D31 of burst2, dn_num = 2*64 + D0, cb wp
        slot1 = slot + 128; /* 128 2*64 num */

        // D0~D31  of  burst3, dn_num = 3*64 + D0, cr wp
        slot2 = slot + 192; /* 192 3*64 num */

        for (i = 0; i <= slice_param->num_ref_idx_l0_active_minus1; i++) {
            d32 = (((td_u8)slice_param->wp_offset_l0_y[i] & 0xff) << 12) | /* 12 num */
                  (((td_u16)slice_param->wp_weight_l0_y[i] & 0x1ff) << 3) | /* 3 num */
                  (slice_param->luma_log2_weight_denom & 0x7);
            WR_MSGWORD(slot0 + i, d32);

            d32 = (((td_u8)slice_param->wp_offset_l0_u[i] & 0xff) << 12) | /* 12 num */
                  (((td_u16)slice_param->wp_weight_l0_u[i] & 0x1ff) << 3) | /* 3 num */
                  (slice_param->chroma_log2_weight_denom & 0x7);
            WR_MSGWORD(slot1 + i, d32);

            d32 = (((td_u8)slice_param->wp_offset_l0_v[i] & 0xff) << 9) | /* 9 num */
                  ((td_u16)slice_param->wp_weight_l0_v[i] & 0x1ff);
            WR_MSGWORD(slot2 + i, d32);
        }

        // list1
        if (g_slice_type_for_pmv[slice_param->slice_type] == 2) { // 2 B Slice
            for (i = 0; i <= slice_param->num_ref_idx_l1_active_minus1; i++) {
                d32 = (((td_u8)slice_param->wp_offset_l1_y[i] & 0xff) << 12) | /* 12 num */
                      (((td_u16)slice_param->wp_weight_l1_y[i] & 0x1ff) << 3) | /* 3 num */
                      (slice_param->luma_log2_weight_denom & 0x7);
                WR_MSGWORD(slot0 + i + 32, d32); /* 32 num */

                d32 = (((td_u8)slice_param->wp_offset_l1_u[i] & 0xff) << 12) | /* 12 num */
                      (((td_u16)slice_param->wp_weight_l1_u[i] & 0x1ff) << 3) | /* 3 num */
                      (slice_param->chroma_log2_weight_denom & 0x7);
                WR_MSGWORD(slot1 + i + 32, d32); /* 32 num */

                d32 = (((td_u8)slice_param->wp_offset_l1_v[i] & 0xff) << 9) | /* 9 num */
                      ((td_u16)slice_param->wp_weight_l1_v[i] & 0x1ff);
                WR_MSGWORD(slot2 + i + 32, d32); /* 32 num */
            }
        }
    }
}

static td_s32 hal_h264_write_fake_slice(td_u8 *slot_vir_addr, td_void *pic_param, td_void *task_mem,
    dec_slice_param_h264 *slice_param, UADDR next_slice_phy_addr)
{
    td_u32 first_mb_in_slice;
    td_u32 last_mb_in_slice;
    UADDR bytes_pos[2] = {0, 0};
    td_u8 bits_offset[2] = {0, 0};
    td_u32 valid_bitlen[2] = {0, 0};

    first_mb_in_slice = slice_param->first_mb_in_slice;
    last_mb_in_slice  = slice_param->last_mb_in_slice;
    valid_bitlen[0]   = slice_param->valid_bitlen[0];
    valid_bitlen[1]   = slice_param->valid_bitlen[1];
    bits_offset[0]     = slice_param->bits_offset[0];
    bits_offset[1]     = slice_param->bits_offset[1];
    bytes_pos[0]       = slice_param->bytes_pos[0];
    bytes_pos[1]       = slice_param->bytes_pos[1];

    slice_param->first_mb_in_slice = 0;
    slice_param->last_mb_in_slice  = first_mb_in_slice - 1;
    slice_param->valid_bitlen[0]   = 1;
    slice_param->valid_bitlen[1]   = 0;
    slice_param->bits_offset[0]     = 0;
    slice_param->bits_offset[1]     = 0;
    slice_param->bytes_pos[0]       = 0;
    slice_param->bytes_pos[1]       = 0;

    if (hal_h264_write_slice_msg(slot_vir_addr, pic_param, task_mem, slice_param, next_slice_phy_addr) != DEC_OK) {
        return DEC_ERR;
    }

    slice_param->first_mb_in_slice = first_mb_in_slice;
    slice_param->last_mb_in_slice  = last_mb_in_slice;
    slice_param->valid_bitlen[0]   = valid_bitlen[0];
    slice_param->valid_bitlen[1]   = valid_bitlen[1];
    slice_param->bits_offset[0]     = bits_offset[0];
    slice_param->bits_offset[1]     = bits_offset[1];
    slice_param->bytes_pos[0]       = bytes_pos[0];
    slice_param->bytes_pos[1]       = bytes_pos[1];

    return DEC_OK;
}


// interface
td_s32 hal_h264_init_hal(td_void *vdh_mem)
{
    vdh_ext_mem_info *ext_mem = (vdh_ext_mem_info *)vdh_mem;

    hal_h264_write_cabac_table(ext_mem->h264_mn_vir_addr);
#ifndef VFMW_PRSAM_FLUSH_ALL_CACHE_SUPPORT
    os_flush_cache(ext_mem->h264_mn_vir_addr, ext_mem->h264_mn_phy_addr, H264_CA_MN_ADDR_LEN);
#endif
    return DEC_OK;
}

td_s32 hal_h264_calc_mem_param(td_void *h264_pic_param, td_void *cfg, td_void *h264_task_mem)
{
    dec_pic_param_h264 *pic_param = (dec_pic_param_h264 *)h264_pic_param;
    dec_chan_mem_param *task_mem  = (dec_chan_mem_param *)h264_task_mem;
    dec_chan_cfg *cfg_ptr = (dec_chan_cfg *)cfg;

    if (pic_param->frame_mbs_only_flag == 0 || pic_param->pic_width_in_mb * 16 <= 1920) { /* 16 1920 num */
        task_mem->vdh_2d_en    = 1;
        task_mem->compress_en = 0;
    } else {
        task_mem->vdh_2d_en    = 0;
        task_mem->compress_en = cfg_ptr->vcmp_mode;
    }

    if (pic_param->pic_width_in_mb > 256 || /* 256 num */
        (pic_param->pic_width_in_mb >= 120 && pic_param->mbaff_frame_flag != 0)) { /* 120 num */
        task_mem->bsp_in_ram = 0;
        task_mem->pxp_in_ram = 0;
    } else {
        task_mem->bsp_in_ram = 1;
        task_mem->pxp_in_ram = 1;
    }

    return DEC_OK;
}

td_s32 hal_h264_process_slice(td_void *h264_pic_param, td_void *h264_slice_head, td_u16 slice_num,
    td_void *slice_list)
{
    dec_pic_param_h264 *pic_param    = (dec_pic_param_h264 *)h264_pic_param;
    dec_slice_param_h264 *slice_head = (dec_slice_param_h264 *)h264_slice_head;

    hal_h264_split_slice_list(pic_param, slice_head, slice_num, slice_list);

    return DEC_OK;
}

static td_void hal_h264_write_reg_cf0_cf1(td_u8 *reg_vir_addr, td_void *h264_pic_param, td_void *h264_task_mem)
{
    dec_pic_param_h264 *pic_param = (dec_pic_param_h264 *)h264_pic_param;
    dec_chan_mem_param *task_mem = (dec_chan_mem_param *)h264_task_mem;
    td_u32 *reg = (td_u32 *)reg_vir_addr;
    td_u32 d32;

    // BASIC_CFG0
    d32 = 0;
    ((VREG_BASIC_CFG0 *)(&d32))->mbamt_to_dec = pic_param->pic_width_in_mb * pic_param->pic_height_in_mb - 1;
    ((VREG_BASIC_CFG0 *)(&d32))->uv_order_en = 0; /* 0 sp420_yuv 1 sp420_yvu */
    ((VREG_BASIC_CFG0 *)(&d32))->load_qmatrix_flag = 1;
    ((VREG_BASIC_CFG0 *)(&d32))->vdh_safe_flag = 0;
    ((VREG_BASIC_CFG0 *)(&d32))->marker_bit_detect_en = 0;
    ((VREG_BASIC_CFG0 *)(&d32))->ac_last_detect_en = 0;
    ((VREG_BASIC_CFG0 *)(&d32))->coef_idx_detect_en = 1;
    ((VREG_BASIC_CFG0 *)(&d32))->vop_type_detect_en = 0;
    ((VREG_BASIC_CFG0 *)(&d32))->work_mode = 0;
    ((VREG_BASIC_CFG0 *)(&d32))->luma_sum_en = 1;
    WR_VREG(reg, VREG_OFS_BASIC_CFG0, d32);

    // BASIC_CFG1
    d32 = 0;
    ((VREG_BASIC_CFG1 *)(&d32))->video_standard      = 0;
    ((VREG_BASIC_CFG1 *)(&d32))->ddr_stride          = 0;
    ((VREG_BASIC_CFG1 *)(&d32))->fst_slc_grp         = 1;
    ((VREG_BASIC_CFG1 *)(&d32))->mv_output_en        = pic_param->nal_ref_idc != 0;
    ((VREG_BASIC_CFG1 *)(&d32))->max_slcgrp_num      = 1;
    ((VREG_BASIC_CFG1 *)(&d32))->line_num_oen        = task_mem->low_delay_en;
    ((VREG_BASIC_CFG1 *)(&d32))->dblk_1d_en          = 1;
    ((VREG_BASIC_CFG1 *)(&d32))->compress_en         = 0;
    ((VREG_BASIC_CFG1 *)(&d32))->ppfd_en             = 0;

    if (task_mem->pmv_phy_addr == 0) {
        ((VREG_BASIC_CFG1 *)(&d32))->mv_output_en = 0;
    }
    WR_VREG(reg, VREG_OFS_BASIC_CFG1, d32);
}

static td_void hal_h264_write_reg_other_1(td_u8 *reg_vir_addr, td_void *h264_pic_param, td_void *h264_task_mem,
    UADDR dn_msg_phyaddr, UADDR up_msg_phy_addr)
{
    dec_pic_param_h264 *pic_param = (dec_pic_param_h264 *)h264_pic_param;
    dec_chan_mem_param *task_mem = (dec_chan_mem_param *)h264_task_mem;
    td_u32 *reg = (td_u32 *)reg_vir_addr;
    td_u32 d32;

#ifndef VFMW_MODULE_LOWDLY_SUPPORT
    vfmw_unused(task_mem);
#endif

    // AVM_ADDR
    d32 = 0;
    ((VREG_AVM_ADDR *)(&d32))->av_msg_addr = dn_msg_phyaddr;
    WR_VREG(reg, VREG_OFS_AVM_ADDR, d32);

    // VAM_ADDR
    d32 = 0;
    ((VREG_VAM_ADDR *)(&d32))->va_msg_addr = up_msg_phy_addr;
    WR_VREG(reg, VREG_OFS_VAM_ADDR, d32);

    // STREAM_BASE_ADDR
    d32 = 0;
    ((VREG_STREAM_BASE_ADDR *)(&d32))->stream_base_addr = pic_param->stream_base_addr & 0xFFFFFFF0;
    WR_VREG(reg, VREG_OFS_STREAM_BASE_ADDR, d32);

    // INT_MASK
#ifdef VFMW_MODULE_LOWDLY_SUPPORT
    d32 = 2; /* 2 mask */
#else
    d32 = 6; /* 6 mask */
#endif
    WR_VREG(reg, VREG_OFS_INT_MASK, d32);

    // TIME_OUT
    d32 = 0x00300C03;
    WR_VREG(reg, VREG_OFS_SED_TO,    d32);
    WR_VREG(reg, VREG_OFS_ITRANS_TO, d32);
    WR_VREG(reg, VREG_OFS_PMV_TO,    d32);
    WR_VREG(reg, VREG_OFS_PRC_TO,    d32);
    WR_VREG(reg, VREG_OFS_RCN_TO,    d32);
    WR_VREG(reg, VREG_OFS_DBLK_TO,   d32);
    WR_VREG(reg, VREG_OFS_PPFD_TO,   d32);

#ifdef VFMW_MODULE_LOWDLY_SUPPORT
    // VREG_OFS_DEC_LEVEL
    d32 = (td_u32)task_mem->tunl_line_num;
    WR_VREG(reg, VREG_OFS_DEC_LEVEL, d32);

    // VREG_OFS_LINE_NUM_STADDR
    d32 = (task_mem->luma_phy_addr + task_mem->line_num_ofs) & 0xFFFFFFF0;
    WR_VREG(reg, VREG_OFS_LINE_NUM_STADDR, d32);
#endif
}

static td_void hal_h264_write_reg_other_2(td_u8 *reg_vir_addr, td_void *h264_pic_param, td_void *h264_task_mem)
{
    dec_pic_param_h264 *pic_param = (dec_pic_param_h264 *)h264_pic_param;
    dec_chan_mem_param *task_mem = (dec_chan_mem_param *)h264_task_mem;
    td_u32 *reg = (td_u32 *)reg_vir_addr;
    td_u32 d32, i;

    // YSTADDR_1D
    d32 = 0;
    ((VREG_YSTADDR_1D *)(&d32))->ystaddr_1d = task_mem->luma_phy_addr & 0xFFFFFFF0;
    WR_VREG(reg, VREG_OFS_YSTADDR_1D, d32);

    // YSTRIDE_1D
    d32 = 0;
    ((VREG_YSTRIDE_1D *)(&d32))->ystride_1d = task_mem->y_stride;
    WR_VREG(reg, VREG_OFS_YSTRIDE_1D, d32);

    // UVOFFSET_1D
    d32 = task_mem->uv_offset;
    WR_VREG(reg, VREG_OFS_UVOFFSET_1D, d32);

    // HEAD_INF_OFFSET
    d32 = 0;
    WR_VREG(reg, VREG_OFS_HEAD_INF_OFFSET, d32);

    // PPFD_BUF_ADDR
    d32 = 0;
    WR_VREG(reg, VREG_OFS_PPFD_BUF_ADDR, d32);

    // PPFD_BUF_LEN
    d32 = 0;
    WR_VREG(reg, VREG_OFS_PPFD_BUF_LEN, d32);

    // REF_PIC_TYPE
    d32 = 0;
    for (i = 0; i < 16; i++) { /* 16 num */
        d32 |= ((pic_param->ref_pic_type[i] & 0x03) << (2 * i)); /* 2 num */
    }
    WR_VREG(reg, VREG_OFS_REF_PIC_TYPE, d32);

    // FF_APT_EN
    d32 = 0;
    WR_VREG(reg, VREG_OFS_FF_APT_EN, d32);

    // CLEAR INT
    d32 = 0xFFFFFFFF;
    WR_VREG(reg, VREG_OFS_INT_STATE, d32);
}

td_s32 hal_h264_write_reg(td_u8 *reg_vir_addr, td_void *h264_pic_param, td_void *h264_task_mem, UADDR dn_msg_phyaddr,
    UADDR up_msg_phy_addr)
{
    hal_h264_write_reg_cf0_cf1(reg_vir_addr, h264_pic_param, h264_task_mem);
    hal_h264_write_reg_other_1(reg_vir_addr, h264_pic_param, h264_task_mem, dn_msg_phyaddr, up_msg_phy_addr);
    hal_h264_write_reg_other_2(reg_vir_addr, h264_pic_param, h264_task_mem);

    return DEC_OK;
}

static td_void hal_h264_write_pic_msg_d0_d9(td_u8 *slot_vir_addr, td_void *h264_pic_param, td_void *h264_task_mem,
    td_void *h264_vdh_mem, td_void *h264_vdh_msg)
{
    dec_pic_param_h264 *pic_param = (dec_pic_param_h264 *)h264_pic_param;
    dec_chan_mem_param *task_mem = (dec_chan_mem_param *)h264_task_mem;
    vdh_ext_mem_info *vdh_mem = (vdh_ext_mem_info *)h264_vdh_mem;
    vdh_msg_info *vdh_msg = (vdh_msg_info *)h264_vdh_msg;
    td_u32 *slot = (td_u32 *)slot_vir_addr;
    td_u32 d32;

    // D0
    d32 = (td_u32)(pic_param->nal_ref_idc != 0) << 31 | /* 31 bit */
          pic_param->cabac_qp_err_dec_en << 30 | /* 30 bit */
          pic_param->transform_8x8_mode_flag << 28 | /* 28 bit */
          pic_param->entropy_coding_mode_flag << 27 | /* 27 bit */
          pic_param->constrained_intra_pred_flag  << 26 | /* 26 bit */
          pic_param->chroma_format_idc << 25 | /* 25 bit */
          (pic_param->pic_height_in_mb - 1) << 16 | /* 16 bit */
          g_struct_trans[pic_param->pic_structure][pic_param->mb_adaptive_frame_field_flag] << 14 | /* 14 bit */
          (pic_param->pic_width_in_mb - 1);
    WR_MSGWORD(slot, d32);

    // D1
    d32 = task_mem->luma_phy_addr & 0xFFFFFFF0;
    WR_MSGWORD(slot + 0x1, d32);

    // D2
    d32 = (td_u32)pic_param->this_poc;
    WR_MSGWORD(slot + 0x2, d32);

    // D3, D4
    if (pic_param->mb_adaptive_frame_field_flag != 0) {
        d32 = (td_u32)pic_param->top_poc;
        WR_MSGWORD(slot + 0x3, d32);

        d32 = (td_u32)pic_param->bottom_poc;
        WR_MSGWORD(slot + 0x4, d32);
    } else {
        d32 = 0;
        WR_MSGWORD(slot + 0x3, d32);
        WR_MSGWORD(slot + 0x4, d32);
    }

    // D5
    d32 = vdh_mem->sed_top_phy_addr & 0xFFFFFFF0;
    WR_MSGWORD(slot + 0x5, d32);

    // D6
    d32 = vdh_mem->pmv_top_phy_addr & 0xFFFFFFF0;
    WR_MSGWORD(slot + 0x6, d32);

    // D7
    if (pic_param->pic_structure != 2) { /* 2 frame or top_field */
        d32 = task_mem->pmv_phy_addr & 0xFFFFFFF0;
    } else {
        d32 = (task_mem->pmv_phy_addr + task_mem->half_pmvblk_offset) & 0xFFFFFFF0;
    }

    WR_MSGWORD(slot + 0x7, d32);

    // D8
    d32 = vdh_mem->rcn_top_phy_addr & 0xFFFFFFF0;
    WR_MSGWORD(slot + 0x8, d32);

    // D9
    d32 = vdh_msg->slice_msg_phy_addr[0] & 0xFFFFFFF0;
    WR_MSGWORD(slot + 0x9, d32);
}

td_s32 hal_h264_write_pic_msg(td_u8 *slot_vir_addr, td_void *h264_pic_param, td_void *h264_task_mem,
    td_void *h264_vdh_mem, td_void *h264_vdh_msg)
{
    dec_pic_param_h264 *pic_param = (dec_pic_param_h264 *)h264_pic_param;
    vdh_ext_mem_info *vdh_mem = (vdh_ext_mem_info *)h264_vdh_mem;
    td_u32 *slot = (td_u32 *)slot_vir_addr;
#ifndef VFMW_PRSAM_FLUSH_ALL_CACHE_SUPPORT
    vdh_msg_info *vdh_msg = (vdh_msg_info *)h264_vdh_msg;
#endif
    td_u32 d32, i;

    hal_h264_write_pic_msg_d0_d9(slot_vir_addr, h264_pic_param, h264_task_mem, h264_vdh_mem, h264_vdh_msg);

    // write D10 ~ D25
    // pic_addr of all apc
    for (i = 0; i < pic_param->apc_size; i++) {
        d32 = pic_param->apc_fsp_phy_addr[i] & 0xFFFFFFF0;
        WR_MSGWORD(slot + 10 + i, d32); /* 10 offset */
    }

    // D26
    d32 = vdh_mem->h264_mn_phy_addr & 0xFFFFFFF0;
    WR_MSGWORD(slot + 26, d32); /* 26 offset */

    // D27~D58
    for (i = 0; i < pic_param->apc_size; i++) {
        d32 = (td_u32)pic_param->apc_poc[0][i];
        WR_MSGWORD(slot + 27 + 2 * i, d32); /* 27 offset 2 mult */

        d32 = (td_u32)pic_param->apc_poc[1][i];
        WR_MSGWORD(slot + 27 + 2 * i + 1, d32); /* 27 offset 2 mult */
    }

    // D59
    d32 = vdh_mem->dblk_top_phy_addr & 0xFFFFFFF0;
    WR_MSGWORD(slot + 59, d32); /* 59 offset */

    // D60
    // vpss_info _addr
    d32 = 0;
    WR_MSGWORD(slot + 60, d32); /* 60 offset */

    // D61
    // Vpss_info_stride
    d32 = 0;
    WR_MSGWORD(slot + 61, d32); /* 61 offset */

    // burst 1
    hal_h264_write_q_matrix(pic_param->qmatrix, 56, slot + 64); /* 56 64 offset */

#ifndef VFMW_PRSAM_FLUSH_ALL_CACHE_SUPPORT
    os_flush_cache(vdh_msg->pic_msg_vir_addr, vdh_msg->pic_msg_phy_addr, H264_PIC_MSG_SLOT_LEN);
#endif
    return DEC_OK;
}

td_s32 hal_h264_write_slice_msg(td_u8 *slot_vir_addr, td_void *h264_pic_param, td_void *task_mem,
    dec_slice_param_h264 *h264_slice_param, UADDR next_slice_phy_addr)
{
    dec_pic_param_h264 *pic_param = (dec_pic_param_h264 *)h264_pic_param;
    dec_slice_param_h264 *slice_param = (dec_slice_param_h264 *)h264_slice_param;
    td_u32 *slot = (td_u32 *)slot_vir_addr;
    td_u32 d32;
    td_s32 result = DEC_ERR;

    // D0 - D5
    result = hal_h264_write_slice_msg_stream_info(pic_param, slice_param, slot);
    if (result == DEC_ERR) {
        return DEC_ERR;
    }

    // D6
    d32 = ((td_u32)slice_param->slice_qpy) << 26 | /* 26 bit */
          ((slice_param->cabac_init_idc & 0x3) << 24) | /* 24 bit */
          (slice_param->first_mb_in_slice & 0xfffff);
    WR_MSGWORD(slot + 6, d32); /* 6 offset */

    // D7
    d32 = ((slice_param->num_ref_idx_l1_active_minus1 & 0x1f) << 21) | /* 21 bit */
          ((slice_param->num_ref_idx_l0_active_minus1 & 0x1f) << 16) | /* 16 bit */
          (slice_param->direct_spatial_mv_pred_flag << 15) | /* 15 bit */
          (slice_param->direct_8x8_inference_flag << 14) | /* 14 bit */
          (slice_param->list_x_size[1] << 8) | /* 8 bit */
          (slice_param->list_x_size[0] << 2) | /* 2 bit */
          g_slice_type_for_pmv[slice_param->slice_type];
    WR_MSGWORD(slot + 7, d32); /* 7 offset */

    // D8 - D9 list[1][0] pmv_addr when B Slice
    hal_h64_write_slice_msg_mv_addr_info(pic_param, task_mem, slice_param, slot);

    // D10
    d32 = (slice_param->weight_flag << 16) | /* 16 bit */
          (((td_u8)slice_param->chroma_qp_index_offset & 0x1f) << 5) | /* 5 bit */
          ((td_u8)slice_param->second_chroma_qp_index_offset & 0x1f);
    WR_MSGWORD(slot + 10, d32); /* 10 offset */

    // D11
    d32 = (((td_u8)slice_param->slice_alpha_c0_offset_div2 & 0xf) << 16) | /* 16 bit */
          (((td_u8)slice_param->slice_beta_offset_div2 & 0xf) << 8) | /* 8 bit */
          slice_param->disable_deblocking_filter_idc;
    WR_MSGWORD(slot + 11, d32); /* 11 offset */

    // D44
    d32 = slice_param->last_mb_in_slice;
    WR_MSGWORD(slot + 44, d32); /* 44 offset */

    // D63 next slice msg addr
    d32 = next_slice_phy_addr & 0xFFFFFFF0;
    WR_MSGWORD(slot + 63, d32); /* 63 offset */

    if (g_slice_type_for_pmv[slice_param->slice_type] != 0) { // P,B Slice
        // D12 - D19, D20 - D35
        hal_h264_write_slice_msg_ref_pic_info(pic_param, slice_param, slot);

        // D36 - D43 apc->refidx
        hal_h264_write_slice_msg_apc_info(pic_param, slice_param, slot);

        // Burst0 - Burst1 weight tab
        hal_h264_write_slice_msg_wp_info(pic_param, slice_param, slot);
    }

    return DEC_OK;
}

td_s32 hal_h264_write_slice_lst(td_void *h264_pic_param, td_void *task_mem, td_void *h264_slice_list,
    td_u8 **slot_vir_addr, UADDR *slot_phyaddr)
{
    dec_pic_param_h264 *pic_param = (dec_pic_param_h264 *)h264_pic_param;
    dec_chan_slice_list *slice_list = (dec_chan_slice_list *)h264_slice_list;
    dec_slice_param_h264 *slice_param = TD_NULL;
    td_u8 *curr_slot_viraddr = TD_NULL;
    UADDR next_slot_phy_addr = 0;
    td_u32 slice_num = 0;
    td_u32 slice_idx = 0;
    td_u32 slice_cnt = 0;

    slice_param = (dec_slice_param_h264 *)slice_list->list_head[0];
    slice_num    = slice_list->list_len[0];

    if (slice_list->fake_slice != 0) {
        dprint(PRN_ERROR, "DEC MB address of the first slice is %d, insert a fake slice!\n",
            slice_param->first_mb_in_slice);

        curr_slot_viraddr = slot_vir_addr[0];
        next_slot_phy_addr  = slot_phyaddr[1];
        slice_cnt++;

        if (hal_h264_write_fake_slice(curr_slot_viraddr, pic_param, task_mem, slice_param, next_slot_phy_addr) !=
            DEC_OK) {
            dprint(PRN_ERROR, "DEC insert a fake slice failed!\n");
            return DEC_ERR;
        }
#ifndef VFMW_PRSAM_FLUSH_ALL_CACHE_SUPPORT
        os_flush_cache(curr_slot_viraddr, slot_phyaddr[0], H264_SLICE_MSG_SLOT_LEN);
#endif
    }

    for (slice_idx = 0; slice_idx < slice_num; slice_cnt++) {
        curr_slot_viraddr = slot_vir_addr[slice_cnt];

        if ((slice_idx == slice_num - 1) || (slice_cnt == H264_MAX_SLICE_NUM - 1)) {
            next_slot_phy_addr = 0;
            slice_idx = slice_num;
        } else {
            next_slot_phy_addr = slot_phyaddr[slice_cnt + 1];
            slice_idx++;
        }

        if (hal_h264_write_slice_msg(curr_slot_viraddr, pic_param, task_mem, slice_param, next_slot_phy_addr) !=
            DEC_OK) {
            return DEC_ERR;
        }
#ifndef VFMW_PRSAM_FLUSH_ALL_CACHE_SUPPORT
        os_flush_cache(curr_slot_viraddr, slot_phyaddr[slice_cnt], H264_SLICE_MSG_SLOT_LEN);
#endif
        slice_param = slice_param->next_slice;
    }

    return DEC_OK;
}

static td_void hal_h264_process_frame_ref(dec_slice_param_h264 *slice_param)
{
    td_u16 ref_idx;
    td_u16 i, j;

    for (i = 0; i < H264_APC_SIZE; i++) {
        ref_idx = 0;

        for (j = 0; j < slice_param->list_x_size[0]; j++) {
            if (i == slice_param->idx_2_apc[0][j]) {
                ref_idx = j;
                break;
            }
        }

        slice_param->apc_2_ref_idx[2 * i] = (td_u8)ref_idx; /* 2 mult */
        slice_param->apc_2_ref_idx[2 * i + 1] = (td_u8)ref_idx; /* 2 mult */
    }
}

static td_void hal_h264_process_field_ref(dec_slice_param_h264 *slice_param)
{
    td_u16 ref_idx;
    td_u16 i, j;

    for (i = 0; i < 2 * H264_APC_SIZE; i++) { /* 2 mult */
        ref_idx = 0;
        for (j = 0; j < slice_param->list_x_size[0]; j++) {
            if (i == ((slice_param->idx_2_apc[0][j] << 1) |
                      (td_u8)(slice_param->declist[0][j].list_structure == 2))) { /* 2 mult */
                ref_idx = j;
                break;
            }
        }

        slice_param->apc_2_ref_idx[i] = (td_u8)ref_idx;
    }
}

static td_void hal_h264_make_apc_2_idx_list0(const dec_pic_param_h264 *pic_param, dec_slice_param_h264 *slice_param)
{
    if (slice_param->slice_type == 2) { // 2 I Slice
        return;
    }

    if (pic_param->pic_structure == 0) { // frame reference
        hal_h264_process_frame_ref(slice_param);
    } else { // field reference
        hal_h264_process_field_ref(slice_param);
    }
}

td_s32 hal_h264_make_slice_ref(td_s32 chan_id, td_void *h264_pic_param, td_void *h264_slice_param)
{
    dec_pic_param_h264 *pic_param   = (dec_pic_param_h264 *)h264_pic_param;
    dec_slice_param_h264 *slice_param = (dec_slice_param_h264 *)h264_slice_param;

    dec_ref_make_idx_2_apc(pic_param->apc_size, &pic_param->apc_fsp_id[0],
        MIN(slice_param->list_x_size[0], H264_APC_SIZE * 2), /* 2 mult */
        &slice_param->idx_2_fsp_id[0][0], &slice_param->idx_2_apc[0][0]);
    dec_ref_make_idx_2_apc(pic_param->apc_size, &pic_param->apc_fsp_id[0],
        MIN(slice_param->list_x_size[1], H264_APC_SIZE * 2), /* 2 mult */
        &slice_param->idx_2_fsp_id[1][0], &slice_param->idx_2_apc[1][0]);

    hal_h264_make_apc_2_idx_list0(pic_param, slice_param);

    dec_ref_make_col_pmv_addr(chan_id, slice_param->col_pmv_fsp_id, &slice_param->apc_2_mv_addr);

    return DEC_OK;
}

STATIC td_s32 hal_h264_get_slice_mb_range_tmp0(td_u32 double_image_width, td_u32 *slice_mb_range_tmp,
    dec_repair_param *repair_param, dec_report_info *dec_report, td_u32 *j)
{
    td_u32 double_lines0, double_lines1, mb_sur_plus0, mb_sur_plus1;

    if ((double_image_width - 2) > (slice_mb_range_tmp[1] % double_image_width)) { /*  NOT end at right most 2 */
        double_lines0 = slice_mb_range_tmp[0] / double_image_width;
        double_lines1 = slice_mb_range_tmp[1] / double_image_width;
        mb_sur_plus0 = (slice_mb_range_tmp[0] % double_image_width) / 2; /* 2 :for half */
        mb_sur_plus1 = (slice_mb_range_tmp[1] % double_image_width) / 2; /* 2 :for half */

        if (((double_lines1 - double_lines0 >= 1) && ((*j) + 3 >= MAX_UP_MSG_SLICE_NUM)) || /* 3 :count number */
            ((double_lines1 - double_lines0 < 1) && ((*j) + 2 >= MAX_UP_MSG_SLICE_NUM))) { /* 2 :count number */
            return DEC_ERR;
        }

        if (double_lines1 - double_lines0 >= 1) {
            dec_report->slice_mb_range[*j][0] = double_lines0 * double_image_width + mb_sur_plus0;
            dec_report->slice_mb_range[*j][1] = double_lines0 * double_image_width +
                (td_u32)repair_param->image_width - 1;
            (*j)++;

            dec_report->slice_mb_range[*j][0] = double_lines0 * double_image_width + (td_u32)repair_param->image_width +
                                               mb_sur_plus0;

            dec_report->slice_mb_range[*j][1] = double_lines1 * double_image_width + mb_sur_plus1;
            (*j)++;

            dec_report->slice_mb_range[*j][0] = double_lines1 * double_image_width + (td_u32)repair_param->image_width;
            dec_report->slice_mb_range[*j][1] = dec_report->slice_mb_range[*j][0] + mb_sur_plus1;
            (*j)++;
        } else {
            dec_report->slice_mb_range[*j][0] = double_lines0 * double_image_width + mb_sur_plus0;
            dec_report->slice_mb_range[*j][1] = double_lines0 * double_image_width + mb_sur_plus1;
            (*j)++;

            dec_report->slice_mb_range[*j][0] =
                double_lines0 * double_image_width + (td_u32)repair_param->image_width + mb_sur_plus0;
            dec_report->slice_mb_range[*j][1] =
                double_lines1 * double_image_width + (td_u32)repair_param->image_width + mb_sur_plus1;
            (*j)++;
        }
    } else { /*  end at right most  */
        if ((*j) + 2 >= MAX_UP_MSG_SLICE_NUM) { /* 2 is algorithm parameters */
            return DEC_ERR;
        }

        double_lines0 = slice_mb_range_tmp[0] / double_image_width;
        mb_sur_plus0 = (slice_mb_range_tmp[0] % double_image_width) / 2; /* 2 :for half */
        dec_report->slice_mb_range[*j][0] = double_lines0 * double_image_width + mb_sur_plus0;
        dec_report->slice_mb_range[*j][1] = double_lines0 * double_image_width + (td_u32)repair_param->image_width - 1;
        (*j)++;

        dec_report->slice_mb_range[*j][0] =
            double_lines0 * double_image_width + (td_u32)repair_param->image_width + mb_sur_plus0;
        dec_report->slice_mb_range[*j][1] = slice_mb_range_tmp[1];
        (*j)++;
    }

    return DEC_OK;
}

STATIC td_s32 hal_h264_get_slice_mb(dec_repair_param *repair_param, dec_report_info *dec_report,
    h264_post *tmp_param, td_u32 slice_mb_range_tmp[2]) /* 2 is an index */
{
    td_u32 j;
    td_u32 double_image_width;
    td_u32 double_lines1;
    td_u32 mb_sur_plus1;

    j = tmp_param->slice_index;
    double_image_width = (td_u32)repair_param->image_width * 2; /* 2 :for double */

    if (double_image_width == 0) {
        return DEC_ERR;
    }

    if ((slice_mb_range_tmp[0] % double_image_width) > 1) { /*  NOT start from left most */
        if (hal_h264_get_slice_mb_range_tmp0(double_image_width,
            slice_mb_range_tmp, repair_param, dec_report, &j) == DEC_ERR) {
            goto too_many_slice;
        }
    } else { /*  start from left most  */
        if ((double_image_width - 2) > (slice_mb_range_tmp[1] % double_image_width)) { /* NOT end at right most 2 */
            if (j + 2 >= MAX_UP_MSG_SLICE_NUM) { /* 2 is algorithm parameters */
                goto too_many_slice;
            }

            double_lines1 = slice_mb_range_tmp[1] / double_image_width;
            mb_sur_plus1 = (slice_mb_range_tmp[1] % double_image_width) / 2; /* 2 :for half */
            dec_report->slice_mb_range[j][0] = slice_mb_range_tmp[0];
            dec_report->slice_mb_range[j][1] = double_lines1 * double_image_width + mb_sur_plus1;
            j++;

            dec_report->slice_mb_range[j][0] = double_lines1 * double_image_width + (td_u32)repair_param->image_width;
            dec_report->slice_mb_range[j][1] = dec_report->slice_mb_range[j][0] + mb_sur_plus1;
            j++;
        } else { /*  end at right most  */
            if (j + 1 >= MAX_UP_MSG_SLICE_NUM) {
                goto too_many_slice;
            }

            dec_report->slice_mb_range[j][0] = slice_mb_range_tmp[0];
            dec_report->slice_mb_range[j][1] = slice_mb_range_tmp[1];
            j++;
        }
    }

    tmp_param->slice_index = j;

    return DEC_OK;

too_many_slice:
    tmp_param->too_many_slice = 1;

    return DEC_ERR;
}

STATIC td_void hal_h264_bubble_sort(dec_report_info *dec_report)
{
    td_u32 i, j;
    td_u32 flag = 1;
    td_u32 swap0, swap1;
    td_u32 n = dec_report->slice_num;

    for (i = 1; (i < n) && (flag == 1); i++) {
        flag = 0;

        for (j = 0; j < n - i; j++) {
            if (dec_report->slice_mb_range[j][0] > dec_report->slice_mb_range[j + 1][0]) {
                flag = 1;
                swap0 = dec_report->slice_mb_range[j][0];
                swap1 = dec_report->slice_mb_range[j][1];
                dec_report->slice_mb_range[j][0] = dec_report->slice_mb_range[j + 1][0];
                dec_report->slice_mb_range[j][1] = dec_report->slice_mb_range[j + 1][1];
                dec_report->slice_mb_range[j + 1][0] = swap0;
                dec_report->slice_mb_range[j + 1][1] = swap1;
            }
        }

        if (flag == 0) {
            return;
        }
    }
}

STATIC td_void hal_h264_get_mb_of_mbaff(dec_repair_param *repair_param, dec_report_info *dec_report,
    const dec_chan_task *task)
{
    td_u32 i;
    td_u32 total_mbn;
    td_u32 dec_slice_num;
    dec_dev_info *dev = TD_NULL;
    vdh_msg_info *msg = TD_NULL;
    h264_post *tmp_param = TD_NULL;

    if (dec_dev_get_dev(task->dev_id, &dev) != DEC_OK ||
        dev->ops->get_msg(dev, task->msg_id, &msg) != DEC_OK) {
        return;
    }

    tmp_param = (h264_post *)(msg->slice_msg_vir_addr);
    tmp_param->slice_index = 0;
    tmp_param->too_many_slice = 0;

    dec_slice_num = dec_report->slice_num;
    total_mbn = (td_u32)(repair_param->image_width * repair_param->image_height);

    for (i = 0; i < dec_slice_num; i++) {
        tmp_param->slice_mb_range[i][0] = dec_report->slice_mb_range[i][0];
        tmp_param->slice_mb_range[i][1] = dec_report->slice_mb_range[i][1];
    }

    for (i = 0; i < dec_slice_num; i++) {
        tmp_param->slice_mb_range[i][0] &= 0xfffffffe;
        tmp_param->slice_mb_range[i][1] |= 0x1;

        if (hal_h264_get_slice_mb(repair_param, dec_report, tmp_param, tmp_param->slice_mb_range[i]) != DEC_OK) {
            break;
        }
    }

    if (tmp_param->too_many_slice == 1) {
        dec_report->slice_num = 1;
        dec_report->slice_mb_range[0][0] = 0;
        dec_report->slice_mb_range[0][1] = total_mbn - 1;
    } else {
        dec_report->slice_num = (td_u16)tmp_param->slice_index;
    }

    hal_h264_bubble_sort(dec_report);
}

STATIC td_void hal_h264_get_mb_group_repair_param(dec_repair_param *repair_param, td_s32 old_slice_start_mbn,
    td_s32 curr_slice_start_mbn, td_s32 old_slice_end_mbn)
{
    repair_param->mb_group[repair_param->valid_group_num].start_mbn =
        (td_u32)(old_slice_start_mbn + (old_slice_start_mbn < 0));
    repair_param->mb_group[repair_param->valid_group_num].end_mbn = (td_u32)(curr_slice_start_mbn - 1);

    if (old_slice_end_mbn > 100 && /*  total_mbn may less than 100  */
        old_slice_end_mbn - 100 >  /* 100 is algorithm parameters */
        (td_s32)repair_param->mb_group[repair_param->valid_group_num].start_mbn) {
        repair_param->mb_group[repair_param->valid_group_num].start_mbn =
            (td_u32)(old_slice_end_mbn - 100); /* 100 is algorithm parameters */
    }

    repair_param->valid_group_num++;
}

STATIC td_s32 hal_h264_get_mb_group_start_and_end_mbn(dec_repair_param *repair_param, dec_report_info *dec_report,
    td_s32 total_mbn)
{
    vfmw_check_ret(repair_param->valid_group_num <= (MAX_UP_MSG_SLICE_NUM - 1), DEC_ERR);

    repair_param->mb_group[repair_param->valid_group_num].start_mbn =
        dec_report->slice_mb_range[dec_report->slice_num - 1][1];
    repair_param->mb_group[repair_param->valid_group_num].end_mbn = (td_u32)(total_mbn - 1);

    repair_param->valid_group_num++;

    return DEC_OK;
}

static td_s32 hal_h264_get_mb_group(dec_repair_param *repair_param, dec_report_info *dec_report,
    const dec_chan_task *task, td_u32 is_mbaff)
{
    td_u32 i;
    td_s32 total_mbn;
    td_s32 old_slice_start_mbn, old_slice_end_mbn, curr_slice_start_mbn, curr_slice_end_mbn;

    if ((dec_report->slice_num <= 0) || (dec_report->slice_num > MAX_UP_MSG_SLICE_NUM)) {
        dprint(PRN_ERROR, "slice_num(=%d) out of range!\n", dec_report->slice_num);
        return DEC_ERR;
    }

    total_mbn = repair_param->image_width * repair_param->image_height;

    if (is_mbaff != 0) {
        hal_h264_get_mb_of_mbaff(repair_param, dec_report, task);
    }

    repair_param->valid_group_num = 0;
    repair_param->full_repair = 0;
    old_slice_start_mbn = -1;
    old_slice_end_mbn = -1;

    dec_report->slice_mb_range[dec_report->slice_num][0] = dec_report->slice_mb_range[dec_report->slice_num][1] =
        (td_u32)total_mbn;

    for (i = 0; i <= dec_report->slice_num; i++) {
        curr_slice_start_mbn = (td_s32)dec_report->slice_mb_range[i][0];
        curr_slice_end_mbn = (td_s32)dec_report->slice_mb_range[i][1];

        if ((curr_slice_start_mbn > total_mbn) || (curr_slice_end_mbn > total_mbn) ||
            (curr_slice_start_mbn > curr_slice_end_mbn)) {
            continue;
        }

        if (old_slice_end_mbn > curr_slice_start_mbn) {
            old_slice_end_mbn = old_slice_start_mbn;
        }

        if ((old_slice_end_mbn + 1) < curr_slice_start_mbn) {
            if (repair_param->valid_group_num > (MAX_UP_MSG_SLICE_NUM - 1)) {
                dprint(PRN_ERROR, "line:%d! repair_param->valid_group_num out of range!\n", __LINE__);
                return DEC_ERR;
            }

            hal_h264_get_mb_group_repair_param(repair_param, old_slice_start_mbn,
                curr_slice_start_mbn, old_slice_end_mbn);
        }

        if (i == ((td_u32)dec_report->slice_num - 1)) {
            if (dec_report->slice_mb_range[dec_report->slice_num - 1][1] < (td_u32)total_mbn - 1) {
                return hal_h264_get_mb_group_start_and_end_mbn(repair_param, dec_report, total_mbn);
            }
        }

        old_slice_start_mbn = curr_slice_start_mbn;
        old_slice_end_mbn = curr_slice_end_mbn;
    }

    return DEC_OK;
}

td_s32 hal_h264_get_rep_param(td_void *repair, const td_void *chan_task, td_void *report)
{
    td_s32 i, ret;
    td_u32 is_mbaff;
    dec_pic_param_h264 *pic_param = TD_NULL;
    const dec_chan_task *task = TD_NULL;
    dec_report_info *dec_report = TD_NULL;
    dec_repair_param *repair_param = TD_NULL;

    task = (dec_chan_task *)chan_task;
    dec_report = (dec_report_info *)report;
    repair_param = (dec_repair_param *)repair;

    vfmw_check_ret((task != TD_NULL), DEC_ERR);
    vfmw_check_ret((dec_report != TD_NULL), DEC_ERR);
    vfmw_check_ret((repair_param != TD_NULL), DEC_ERR);

    vfmw_check_ret((task->dps != TD_NULL), DEC_ERR);
    pic_param = (dec_pic_param_h264 *)task->dps->pic_param;
    vfmw_check_ret((pic_param != TD_NULL), DEC_ERR);
    repair_param->image_addr = task->mem_param.luma_phy_addr;
    repair_param->ref_image_addr = pic_param->apc_fsp_phy_addr[0];
    repair_param->image_width = (td_s32)pic_param->pic_width_in_mb;
    repair_param->image_height = (td_s32)pic_param->pic_height_in_mb;
    repair_param->is_frame = (pic_param->pic_structure == 0);
    repair_param->image_csp = (pic_param->chroma_format_idc == 0);
    repair_param->rpr_ref_pic_type = pic_param->ref_pic_type[0];

    if (repair_param->image_addr == repair_param->ref_image_addr && pic_param->apc_size > 0) {
        for (i = 0; i < pic_param->apc_size && i < H264_APC_SIZE; i++) {
            if (repair_param->image_addr != pic_param->apc_fsp_phy_addr[i]) {
                repair_param->ref_image_addr = pic_param->apc_fsp_phy_addr[i];
                break;
            }
        }
    }

    if (pic_param->pic_structure == 0) {
        repair_param->pic_type = 0;
    } else if (pic_param->pic_structure == 1) {
        repair_param->pic_type = 1;
    } else if (pic_param->pic_structure == 2) { /* 2 is a type */
        repair_param->pic_type = 2; /* 2 is a type */
    } else if (pic_param->pic_structure == 3) { /* 3 is a type */
        repair_param->pic_type = 0;
    }

    repair_param->ctb_size = 4; /* 4:mem size: */
    repair_param->bit_depth = 8; /* 8 is BitWidth */
    repair_param->vdh_2d_en = task->mem_param.vdh_2d_en;

    is_mbaff = (pic_param->mb_adaptive_frame_field_flag != 0) && (pic_param->pic_structure == 0);
    ret = hal_h264_get_mb_group(repair_param, dec_report, task, is_mbaff);

    return ret;
}
