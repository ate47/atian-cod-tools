#include <includes.hpp>
#include <core/config.hpp>
#include <hook/module_mapper.hpp>
#include "error_coder.hpp"
#include "tools/tools_ui.hpp"
#include "tools/tools_nui.hpp"

namespace error_coder {

	const uint32_t word_sizes[] = { 6, 10, 8, 8 };
	const char* words[4][1024] = {
		{
			"Spring",
			"Juliett",
			"Zed",
			"XRay",
			"charlie",
			"Summer",
			"Edward",
			"Nora",
			"Frank",
			"Uniform",
			"Adam",
			"David",
			"East",
			"Yorker",
			"Prep",
			"Collar",
			"Kilo",
			"Papa",
			"South",
			"Golf",
			"Echo",
			"Vic",
			"June",
			"Tango",
			"Union",
			"Ocean",
			"Victor",
			"Lima",
			"Sierra",
			"Fall",
			"Winter",
			"Baker",
			"Roger",
			"Hotel",
			"Ida",
			"West",
			"Yankee",
			"Sail",
			"Oscar",
			"King",
			"Mike",
			"Young",
			"Option",
			"Whiskey",
			"Zulu",
			"Cast",
			"India",
			"Dog",
			"Romeo",
			"Vice",
			"Boy",
			"Foxtrot",
			"Quebec",
			"Queen",
			"North",
			"Negative",
			"Rush",
			"Delta",
			"May",
			"Zebra",
			"Alfa",
			"Tommy",
			"November",
			"Bravo",
		},
		{
			"43",
			"648",
			"584",
			"51",
			"871",
			"342",
			"455",
			"216",
			"137",
			"680",
			"211",
			"866",
			"971",
			"641",
			"776",
			"188",
			"496",
			"62",
			"985",
			"510",
			"117",
			"284",
			"923",
			"23",
			"445",
			"273",
			"1006",
			"202",
			"852",
			"537",
			"425",
			"230",
			"195",
			"310",
			"708",
			"779",
			"41",
			"620",
			"13",
			"643",
			"443",
			"388",
			"259",
			"520",
			"172",
			"107",
			"471",
			"250",
			"784",
			"5",
			"459",
			"740",
			"725",
			"93",
			"220",
			"796",
			"969",
			"622",
			"452",
			"601",
			"603",
			"931",
			"518",
			"499",
			"515",
			"22",
			"666",
			"105",
			"894",
			"566",
			"942",
			"785",
			"958",
			"890",
			"20",
			"578",
			"426",
			"531",
			"881",
			"436",
			"120",
			"248",
			"252",
			"517",
			"550",
			"735",
			"609",
			"823",
			"899",
			"218",
			"26",
			"859",
			"747",
			"300",
			"403",
			"543",
			"659",
			"673",
			"304",
			"777",
			"676",
			"78",
			"686",
			"10",
			"625",
			"324",
			"264",
			"186",
			"821",
			"677",
			"168",
			"461",
			"754",
			"246",
			"528",
			"249",
			"807",
			"215",
			"289",
			"160",
			"492",
			"225",
			"101",
			"936",
			"367",
			"532",
			"355",
			"345",
			"350",
			"613",
			"538",
			"50",
			"17",
			"768",
			"804",
			"813",
			"295",
			"411",
			"993",
			"914",
			"179",
			"524",
			"213",
			"1001",
			"979",
			"282",
			"87",
			"134",
			"453",
			"695",
			"347",
			"232",
			"233",
			"228",
			"45",
			"1009",
			"126",
			"262",
			"968",
			"31",
			"874",
			"99",
			"155",
			"361",
			"617",
			"222",
			"633",
			"828",
			"16",
			"671",
			"957",
			"965",
			"717",
			"737",
			"567",
			"122",
			"383",
			"724",
			"699",
			"687",
			"955",
			"741",
			"321",
			"853",
			"693",
			"279",
			"185",
			"56",
			"391",
			"876",
			"303",
			"526",
			"731",
			"55",
			"154",
			"951",
			"147",
			"257",
			"712",
			"239",
			"694",
			"111",
			"561",
			"92",
			"170",
			"941",
			"6",
			"577",
			"327",
			"3",
			"363",
			"196",
			"64",
			"963",
			"341",
			"727",
			"392",
			"408",
			"336",
			"698",
			"187",
			"400",
			"544",
			"449",
			"549",
			"21",
			"827",
			"54",
			"148",
			"582",
			"207",
			"745",
			"288",
			"333",
			"271",
			"198",
			"418",
			"237",
			"970",
			"795",
			"901",
			"475",
			"743",
			"808",
			"588",
			"917",
			"417",
			"451",
			"493",
			"469",
			"683",
			"997",
			"786",
			"47",
			"696",
			"885",
			"615",
			"935",
			"266",
			"921",
			"551",
			"904",
			"880",
			"678",
			"709",
			"948",
			"481",
			"564",
			"474",
			"569",
			"457",
			"253",
			"715",
			"700",
			"75",
			"204",
			"146",
			"317",
			"939",
			"311",
			"265",
			"864",
			"704",
			"912",
			"530",
			"954",
			"908",
			"423",
			"244",
			"484",
			"651",
			"618",
			"710",
			"489",
			"373",
			"380",
			"571",
			"587",
			"14",
			"11",
			"67",
			"125",
			"581",
			"166",
			"988",
			"952",
			"546",
			"399",
			"432",
			"626",
			"402",
			"497",
			"331",
			"103",
			"654",
			"773",
			"857",
			"334",
			"972",
			"270",
			"663",
			"1",
			"1020",
			"49",
			"32",
			"674",
			"142",
			"419",
			"994",
			"429",
			"771",
			"500",
			"634",
			"646",
			"70",
			"841",
			"29",
			"614",
			"298",
			"503",
			"219",
			"174",
			"332",
			"129",
			"719",
			"640",
			"119",
			"90",
			"406",
			"290",
			"594",
			"194",
			"91",
			"900",
			"177",
			"752",
			"956",
			"631",
			"730",
			"354",
			"850",
			"539",
			"763",
			"34",
			"466",
			"312",
			"767",
			"940",
			"653",
			"15",
			"845",
			"583",
			"723",
			"554",
			"669",
			"635",
			"112",
			"559",
			"66",
			"749",
			"164",
			"548",
			"309",
			"159",
			"71",
			"353",
			"269",
			"604",
			"934",
			"169",
			"138",
			"883",
			"165",
			"382",
			"52",
			"176",
			"79",
			"231",
			"1014",
			"374",
			"933",
			"444",
			"77",
			"875",
			"984",
			"261",
			"947",
			"74",
			"533",
			"434",
			"992",
			"751",
			"689",
			"338",
			"458",
			"439",
			"930",
			"576",
			"267",
			"258",
			"184",
			"616",
			"132",
			"1010",
			"256",
			"608",
			"540",
			"65",
			"839",
			"463",
			"746",
			"38",
			"460",
			"369",
			"834",
			"847",
			"421",
			"89",
			"863",
			"872",
			"928",
			"221",
			"251",
			"579",
			"95",
			"769",
			"171",
			"789",
			"313",
			"924",
			"733",
			"682",
			"294",
			"375",
			"563",
			"501",
			"210",
			"104",
			"263",
			"133",
			"470",
			"394",
			"662",
			"19",
			"1013",
			"652",
			"203",
			"810",
			"68",
			"962",
			"814",
			"167",
			"274",
			"960",
			"28",
			"440",
			"412",
			"435",
			"1000",
			"623",
			"877",
			"340",
			"319",
			"838",
			"140",
			"37",
			"959",
			"328",
			"276",
			"305",
			"427",
			"485",
			"25",
			"384",
			"108",
			"381",
			"681",
			"729",
			"553",
			"178",
			"621",
			"366",
			"306",
			"642",
			"370",
			"163",
			"800",
			"438",
			"791",
			"711",
			"595",
			"433",
			"591",
			"529",
			"637",
			"667",
			"702",
			"867",
			"260",
			"720",
			"30",
			"161",
			"245",
			"182",
			"938",
			"109",
			"143",
			"285",
			"278",
			"448",
			"401",
			"398",
			"568",
			"573",
			"296",
			"560",
			"76",
			"193",
			"893",
			"339",
			"738",
			"966",
			"85",
			"479",
			"650",
			"1005",
			"60",
			"150",
			"127",
			"840",
			"896",
			"513",
			"8",
			"565",
			"627",
			"798",
			"990",
			"59",
			"961",
			"884",
			"750",
			"761",
			"157",
			"638",
			"830",
			"96",
			"358",
			"349",
			"644",
			"301",
			"223",
			"292",
			"389",
			"362",
			"387",
			"446",
			"254",
			"118",
			"480",
			"110",
			"759",
			"732",
			"949",
			"919",
			"552",
			"27",
			"149",
			"507",
			"156",
			"706",
			"102",
			"236",
			"778",
			"1007",
			"909",
			"986",
			"926",
			"599",
			"326",
			"953",
			"898",
			"329",
			"889",
			"858",
			"450",
			"675",
			"574",
			"753",
			"882",
			"790",
			"787",
			"861",
			"100",
			"519",
			"685",
			"610",
			"824",
			"977",
			"192",
			"628",
			"136",
			"454",
			"657",
			"385",
			"498",
			"873",
			"139",
			"846",
			"486",
			"243",
			"809",
			"668",
			"414",
			"762",
			"980",
			"1008",
			"920",
			"575",
			"781",
			"688",
			"1003",
			"478",
			"713",
			"690",
			"655",
			"703",
			"805",
			"316",
			"242",
			"665",
			"227",
			"597",
			"521",
			"173",
			"967",
			"607",
			"870",
			"322",
			"523",
			"39",
			"422",
			"352",
			"344",
			"592",
			"820",
			"636",
			"803",
			"404",
			"600",
			"214",
			"315",
			"162",
			"534",
			"1012",
			"802",
			"516",
			"462",
			"793",
			"337",
			"84",
			"456",
			"855",
			"390",
			"782",
			"277",
			"831",
			"818",
			"505",
			"153",
			"512",
			"199",
			"302",
			"892",
			"197",
			"780",
			"482",
			"964",
			"792",
			"734",
			"494",
			"946",
			"995",
			"728",
			"128",
			"714",
			"283",
			"48",
			"291",
			"40",
			"684",
			"72",
			"365",
			"124",
			"765",
			"522",
			"226",
			"658",
			"379",
			"632",
			"915",
			"18",
			"913",
			"1016",
			"413",
			"572",
			"679",
			"405",
			"996",
			"837",
			"97",
			"217",
			"488",
			"586",
			"158",
			"473",
			"536",
			"887",
			"879",
			"764",
			"1019",
			"377",
			"721",
			"308",
			"130",
			"224",
			"247",
			"205",
			"602",
			"348",
			"868",
			"974",
			"372",
			"832",
			"275",
			"477",
			"525",
			"371",
			"950",
			"829",
			"978",
			"843",
			"131",
			"318",
			"664",
			"596",
			"504",
			"180",
			"611",
			"811",
			"509",
			"320",
			"844",
			"346",
			"527",
			"918",
			"360",
			"886",
			"826",
			"416",
			"490",
			"281",
			"141",
			"812",
			"287",
			"670",
			"848",
			"756",
			"891",
			"1023",
			"424",
			"468",
			"833",
			"907",
			"927",
			"2",
			"116",
			"744",
			"255",
			"240",
			"420",
			"113",
			"83",
			"297",
			"212",
			"121",
			"36",
			"299",
			"1022",
			"661",
			"280",
			"739",
			"395",
			"35",
			"598",
			"1018",
			"508",
			"862",
			"806",
			"511",
			"181",
			"888",
			"1015",
			"467",
			"905",
			"630",
			"152",
			"323",
			"234",
			"24",
			"396",
			"241",
			"558",
			"330",
			"937",
			"229",
			"701",
			"63",
			"293",
			"656",
			"356",
			"943",
			"945",
			"895",
			"487",
			"722",
			"716",
			"989",
			"758",
			"88",
			"825",
			"906",
			"123",
			"903",
			"570",
			"922",
			"351",
			"639",
			"368",
			"502",
			"545",
			"605",
			"33",
			"929",
			"541",
			"757",
			"73",
			"983",
			"201",
			"580",
			"9",
			"619",
			"869",
			"145",
			"209",
			"775",
			"69",
			"343",
			"437",
			"441",
			"183",
			"81",
			"910",
			"1004",
			"629",
			"766",
			"645",
			"1017",
			"364",
			"58",
			"80",
			"726",
			"415",
			"61",
			"660",
			"144",
			"801",
			"788",
			"797",
			"748",
			"12",
			"897",
			"612",
			"286",
			"585",
			"718",
			"772",
			"902",
			"86",
			"535",
			"556",
			"1011",
			"697",
			"865",
			"815",
			"191",
			"854",
			"46",
			"816",
			"431",
			"944",
			"235",
			"999",
			"483",
			"335",
			"911",
			"151",
			"774",
			"736",
			"53",
			"514",
			"647",
			"430",
			"1021",
			"835",
			"849",
			"410",
			"755",
			"975",
			"325",
			"1002",
			"692",
			"189",
			"991",
			"819",
			"590",
			"973",
			"98",
			"357",
			"822",
			"799",
			"114",
			"94",
			"932",
			"4",
			"981",
			"386",
			"842",
			"982",
			"57",
			"547",
			"42",
			"495",
			"106",
			"491",
			"378",
			"464",
			"407",
			"393",
			"817",
			"742",
			"760",
			"314",
			"442",
			"397",
			"465",
			"476",
			"770",
			"705",
			"1024",
			"175",
			"82",
			"624",
			"208",
			"268",
			"606",
			"44",
			"976",
			"359",
			"555",
			"851",
			"428",
			"472",
			"272",
			"7",
			"206",
			"135",
			"691",
			"860",
			"649",
			"557",
			"878",
			"115",
			"190",
			"447",
			"376",
			"562",
			"542",
			"506",
			"409",
			"794",
			"916",
			"783",
			"707",
			"238",
			"856",
			"925",
			"672",
			"307",
			"836",
			"987",
			"200",
			"593",
			"589",
			"998"
		},
		{
			"Yellow",
			"Green",
			"Blue",
			"Violet",
			"Red",
			"Orange",
			"United",
			"Arctic",
			"Skyline",
			"Cruising",
			"Riptide",
			"Volcanic",
			"Desert",
			"Swamp",
			"Tropic",
			"Hidden",
			"Found",
			"Mountain",
			"Small",
			"Grey",
			"Tan",
			"Ghost",
			"Brass",
			"Silver",
			"Gold",
			"Copper",
			"Gray",
			"Earth",
			"Water",
			"Fire",
			"Air",
			"Sky",
			"Good",
			"Dangerous",
			"Urban",
			"Iron",
			"Nickel",
			"Titanium",
			"Tin",
			"Marble",
			"Platinum",
			"Bayou",
			"River",
			"Marsh",
			"Bog",
			"Battle",
			"War",
			"Embattled",
			"Empty",
			"Dark",
			"Bad",
			"Old",
			"Kinetic",
			"Deadly",
			"Lost",
			"Rare",
			"Melted",
			"Evil",
			"Tragic",
			"Acid",
			"Bogus",
			"Cruel",
			"Ancient",
			"Shellshocked",
			"Wartorn",
			"Numbered",
			"Tall",
			"Fake",
			"Null",
			"Conflicted",
			"Crimson",
			"Prepared",
			"Soaring",
			"Weak",
			"Jaded",
			"Recovered",
			"Calm",
			"Camouflaged",
			"Apocalyptic",
			"Grounded",
			"Screaming",
			"Undercover",
			"Quick",
			"Loud",
			"Wild",
			"Mad",
			"Blind",
			"Chief",
			"Dual",
			"Nova",
			"Fast",
			"Cut",
			"Barricaded",
			"Pale",
			"Left",
			"Right",
			"Hurt",
			"Dry",
			"Flat",
			"Salt",
			"Shade",
			"Sick",
			"Metallic",
			"Metal",
			"Vicious",
			"Boiling",
			"Classic",
			"Operation",
			"Ballistic",
			"Cloaked",
			"Disguised",
			"Guerrilla",
			"Major",
			"Military",
			"Patient",
			"Fierce",
			"Solo",
			"Burned",
			"Scorched",
			"Central",
			"Clean",
			"Covert",
			"Sleeper",
			"Spotted",
			"Grizzly",
			"Snow",
			"Lone",
			"Nuclear",
			"Risky",
			"Broken",
			"Gothic",
			"Apollo",
			"Taz",
			"Brimstone",
			"Firebrand",
			"Torched",
			"Light",
			"Screech",
			"TD",
			"Bolt",
			"Knockout",
			"Burner",
			"Burn",
			"Divebomb",
			"Blink",
			"Snap",
			"Roadrunner",
			"Furious",
			"Hannibal",
			"Clockwork",
			"Strange",
			"Ace",
			"Nightshade",
			"Grim",
			"Knight",
			"Max",
			"Nitro",
			"Resolute",
			"Shadow",
			"Atomic",
			"Titanic",
			"Everest",
			"Bigfoot",
			"Steel",
			"Tough",
			"Motor",
			"Chopper",
			"Rocky",
			"Wicked",
			"Midnight",
			"Morning",
			"Satellite",
			"Extinct",
			"Brute",
			"Diamond",
			"Frosty",
			"Frozen",
			"Miami",
			"Polar",
			"Sub-zero",
			"Sapphire",
			"Doomsday",
			"Monster",
			"Blazing",
			"Blood",
			"October",
			"Emerald",
			"Almighty",
			"Mean",
			"Poison",
			"Electric",
			"Solar",
			"Flying",
			"Mythic",
			"Ultraviolet",
			"Axiom",
			"Heavy",
			"Champagne",
			"Great",
			"Golden",
			"Midas",
			"Whisky",
			"December",
			"Due",
			"Aurora",
			"Andromeda",
			"Corrupt",
			"General",
			"Binary",
			"Lying",
			"Unfriendly",
			"Goodbye",
			"Meridian",
			"Victorious",
			"Barracuda",
			"Chrome",
			"Crystal",
			"Goliath",
			"Sandy",
			"Winning",
			"World",
			"Area",
			"Positive",
			"Dazed",
			"Confused",
			"Rightful",
			"Proper",
			"Foolish",
			"Defiant",
			"Early",
			"Late",
			"Advanced",
			"Aggressive",
			"Silent",
			"Stealth",
			"Natural",
			"Vengeful",
			"Swift",
			"Extreme",
			"Determined",
			"Discovered",
			"Unlimited",
			"Explosive",
			"Rapid",
			"Armored",
			"Destructive",
			"Silenced",
			"Invisible",
			"Clipped",
			"Reloaded",
			"Revolutionary",
			"Muzzled",
			"Divided",
			"Split",
			"Barren",
			"Desolate",
		},
		{
			"Wolf",
			"Gator",
			"Eel",
			"Bear",
			"Octopus",
			"Wombat",
			"Boar",
			"Rattlesnake",
			"Rook",
			"Hornet",
			"Tornado",
			"Viper",
			"Cobra",
			"Dove",
			"Lion",
			"Leopard",
			"Badger",
			"Axe",
			"Tiger",
			"Eagle",
			"Rhino",
			"Gorilla",
			"Lizard",
			"Coyote",
			"Owl",
			"Vulture",
			"Raven",
			"Elephant",
			"Greyhound",
			"Hound",
			"Snake",
			"Thunder",
			"Demon",
			"Rookie",
			"Spartan",
			"Aquarius",
			"Pisces",
			"Aries",
			"Taurus",
			"Gemini",
			"Cancer",
			"Leo",
			"Virgo",
			"Libra",
			"Scorpio",
			"Sagittarius",
			"Capricorn",
			"Freedom",
			"Fog",
			"Avalanche",
			"Cyclone",
			"Sledgehammer",
			"Giant",
			"Zombie",
			"Shark",
			"Puma",
			"Stallion",
			"Werewolf",
			"Hippo",
			"Cougar",
			"Eclipse",
			"Contingency",
			"Python",
			"Boa",
			"Jaguar",
			"Valentine",
			"Bison",
			"Wolverine",
			"Crocodile",
			"Alligator",
			"Rat",
			"Ant",
			"Devil",
			"Scorpion",
			"Goat",
			"Mamba",
			"Dragon",
			"Buffalo",
			"Wasp",
			"Obelisk",
			"Mess",
			"Conflict",
			"Pig",
			"Storm",
			"Blizzard",
			"Menace",
			"Phantom",
			"Marauder",
			"Operator",
			"Arrow",
			"Bayonet",
			"Brigade",
			"Cadet",
			"Camo",
			"Carrier",
			"Command",
			"Convoy",
			"Corps",
			"Crew",
			"Defense",
			"Division",
			"Fleet",
			"Force",
			"Formation",
			"Fort",
			"Garrison",
			"Grunt",
			"Headquarters",
			"Helmet",
			"Honor",
			"Infantry",
			"Insignia",
			"Intelligence",
			"Invasion",
			"Jet",
			"Medic",
			"Medal",
			"Manuever",
			"Missile",
			"Mortar",
			"Munition",
			"Security",
			"Navy",
			"Ordnance",
			"Parachute",
			"Plane",
			"Platoon",
			"Boat",
			"Ship",
			"Radar",
			"Rank",
			"Locker",
			"Recruit",
			"Rescue",
			"Retreat",
			"Artillery",
			"Sabotage",
			"Salute",
			"Salvo",
			"Section",
			"Service",
			"Shell",
			"Siege",
			"Soldier",
			"Specialist",
			"Squad",
			"Squadron",
			"Stockade",
			"Tactics",
			"Theater",
			"Volley",
			"Potato",
			"Agent",
			"Asset",
			"Bridge",
			"Case",
			"Cipher",
			"Code",
			"Cover",
			"Escort",
			"Ghoul",
			"Contraband",
			"Legend",
			"Mole",
			"Parole",
			"Pattern",
			"Station",
			"Company",
			"Drop",
			"Traffic",
			"Ultra",
			"Uncle",
			"Bug",
			"Overlord",
			"Spear",
			"Fury",
			"Claw",
			"Justice",
			"Curtain",
			"Cause",
			"Beastmaster",
			"Rage",
			"Linebacker",
			"Guardian",
			"Serpent",
			"Cage",
			"Breakfast",
			"Mauler",
			"Rodeo",
			"Centaur",
			"Abyss",
			"Bandit",
			"Dementor",
			"Godfather",
			"Mirage",
			"Reaper",
			"Tank",
			"Gladiator",
			"Zeus",
			"Payday",
			"Apocalypse",
			"Conquest",
			"Victory",
			"Undertow",
			"Firestarter",
			"Gremlin",
			"Goblin",
			"Crossbones",
			"Fang",
			"Rain",
			"Whisper",
			"Noise",
			"Gravedigger",
			"Dynamite",
			"Highway",
			"Pavement",
			"Ice",
			"Wave",
			"Clover",
			"Lantern",
			"Sunrise",
			"Fox",
			"Karma",
			"Haze",
			"Scythe",
			"Razor",
			"Honey",
			"Jackpot",
			"Luck",
			"Star",
			"Yeti",
			"Discovery",
			"Mustang",
			"Outlander",
			"Beast",
			"Maelstrom",
			"Lamprey",
			"Absolution",
			"Horizon",
			"Safety",
			"Activity",
			"Outlaw",
			"Tide",
			"Dawn",
			"Goodnight",
			"Express",
			"Moon",
			"Orion",
			"Leviathan",
			"Treasure",
			"Homerun",
			"Refuge",
			"Management",
			"Doppelganger",
			"Eye",
			"Chance",
		}
	};

	void Encode(ErrorCode& code, uint32_t val) {
		for (size_t i = 0; i < ARRAYSIZE(word_sizes); i++) {
			int log2Size = word_sizes[i];
			int row = (int)(val & ((1 << log2Size) - 1));
			val >>= log2Size;

			auto v = words[i][row];

			code[i] = v;
		}
	}

	uint32_t Decode(const ErrorCode& code) {
		if (!code[0] || !code[1] || !code[2] || !code[3]) {
			return 0;
		}

		uint32_t output{};
		for (int i = 3; i >= 0; i--) {
			auto begin = std::begin(words[i]);
			auto end = std::begin(words[i]) + (1ull << word_sizes[i]);
			auto* c = code[i];
			
			auto it = std::find_if(begin, end, [c](const char* w) { return !_strcmpi(c, w); });

			if (it == end) {
				throw std::exception(utils::va("Unknown word '%s'", code[i]));
			}

			auto idx = it - begin;

			output |= idx;

			if (i > 0) {
				output <<= word_sizes[i - 1];
			}
		}

		return output;
	}

	const char* ToStr(const ErrorCode& code) {
		return utils::va("%s %s %s %s", code[0], code[1], code[2], code[3]);
	}
}

namespace {
	int errenc(Process& proc, int argc, const char* argv[]) {
		if (argc < 3) {
			return tool::BAD_USAGE;
		}
		error_coder::ErrorCode code{};

		for (size_t i = 2; i < argc; i++) {
			uint32_t codedec = (uint32_t)std::strtoull(argv[i], nullptr, 10);

			error_coder::Encode(code, codedec);

			LOG_INFO("{}={}", codedec, error_coder::ToStr(code));
		}

		return tool::OK;
	}

	int errdec(Process& proc, int argc, const char* argv[]) {
		if (argc < 6) {
			return tool::BAD_USAGE;
		}
		error_coder::ErrorCode code{};

		code[0] = argv[2];
		code[1] = argv[3];
		code[2] = argv[4];
		code[3] = argv[5];

		uint32_t codedec;

		try {
			codedec = error_coder::Decode(code);
		}
		catch (std::exception& exp) {

			LOG_ERROR("Can't decode code: {}", exp.what());
			return tool::BASIC_ERROR;
		}

		LOG_INFO("{}={}", codedec, error_coder::ToStr(code));

		return tool::OK;
	}


	struct {
		std::string encodeStr{};
		std::string decodeStr{};
		HWND titleLabel{};

		HWND encodeEdit{};
		HWND encodeResEdit{};
		HWND encodeEditLabel{};

		HWND decodeEdit{};
		HWND decodeResEdit{};
		HWND decodeEditLabel{};
	} info{};

	void ComputeDecode() {
		info.decodeStr = utils::WStrToStr(tool::ui::GetWindowTextVal(info.decodeEdit));

		if (info.decodeStr.empty()) {
			Edit_SetText(info.decodeResEdit, "");
			return;
		}

		std::string words[4]{};

		try {
			size_t idx{};
			for (int i = 0; i < 4; i++) {
				size_t f = info.decodeStr.find(' ', idx);
				if (f == std::string::npos) {
					if (i != 3) {
						Edit_SetText(info.decodeResEdit, "Not enough components");
						return;
					}
					f = info.decodeStr.length();
				}
				if (f - idx == 0) {
					// empty word
					i--;
					idx = f + 1;
					continue;
				}

				words[i] = info.decodeStr.substr(idx, f - idx);
				idx = f + 1;
			}
			while (idx < info.decodeStr.length() && info.decodeStr[idx] == ' ') {
				idx++; // ignore end spaces
			}
			if (idx < info.decodeStr.length()) {
				Edit_SetText(info.decodeResEdit, "Too many components");
				return;
			}

			error_coder::ErrorCode code{ words[0].c_str(), words[1].c_str(), words[2].c_str(), words[3].c_str() };

			uint32_t err = error_coder::Decode(code);

			std::string res = std::format("{}", err);
			Edit_SetText(info.decodeResEdit, res.c_str());
		}
		catch (std::exception& e) {
			std::string res = std::format("{}", e.what());
			Edit_SetText(info.decodeResEdit, res.c_str());
		}
	}
	void ComputeEncode() {
		info.encodeStr = utils::WStrToStr(tool::ui::GetWindowTextVal(info.encodeEdit));

		if (info.encodeStr.empty()) {
			Edit_SetText(info.encodeResEdit, "");
			return;
		}

		try {
			const char* cstr = info.encodeStr.c_str();
			char* end;
			uint32_t err = std::strtoul(cstr, &end, 10);
			if (end != cstr + info.encodeStr.length()) {
				throw std::runtime_error("Invalid code");
			}

			error_coder::ErrorCode code{};
			error_coder::Encode(code, err);

			std::string res = error_coder::ToStr(code);
			Edit_SetText(info.encodeResEdit, res.c_str());
		}
		catch (std::exception& e) {
			std::string res = std::format("{}", e.what());
			Edit_SetText(info.encodeResEdit, res.c_str());
		}
	}

	int Render(HWND window, HINSTANCE hInstance) {
		std::wstring encw = utils::StrToWStr(info.encodeStr);
		std::wstring decw = utils::StrToWStr(info.decodeStr);

		info.titleLabel = CreateWindowExW(
			0,
			L"STATIC",
			L"Error coder",
			SS_CENTER | WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		info.encodeEdit = CreateWindowExW(
			0,
			L"EDIT",
			encw.c_str(),
			WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		info.encodeResEdit = CreateWindowExW(
			0,
			L"EDIT",
			L"",
			WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		info.encodeEditLabel = CreateWindowExW(
			0,
			L"STATIC",
			L"Encode : ",
			SS_RIGHT | WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		info.decodeEdit = CreateWindowExW(
			0,
			L"EDIT",
			decw.c_str(),
			WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		info.decodeResEdit = CreateWindowExW(
			0,
			L"EDIT",
			L"",
			WS_BORDER | WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		info.decodeEditLabel = CreateWindowExW(
			0,
			L"STATIC",
			L"Decode : ",
			SS_RIGHT | WS_CHILD | WS_VISIBLE,
			0, 0, 0, 0,
			window,
			NULL,
			hInstance,
			NULL
		);

		if (
			info.encodeEdit == NULL
			|| info.encodeResEdit == NULL
			|| info.decodeEdit == NULL
			|| info.decodeResEdit == NULL
			|| info.encodeEditLabel == NULL
			|| info.decodeEditLabel == NULL
			|| info.titleLabel == NULL
			) {
			return -1;
		}

		SendMessage(info.encodeEdit, EM_SETLIMITTEXT, (WPARAM)MAX_PATH, (LPARAM)0);
		SendMessage(info.decodeEdit, EM_SETLIMITTEXT, (WPARAM)MAX_PATH, (LPARAM)0);
		SendMessage(info.encodeResEdit, EM_SETLIMITTEXT, (WPARAM)MAX_PATH, (LPARAM)0);
		SendMessage(info.decodeResEdit, EM_SETLIMITTEXT, (WPARAM)MAX_PATH, (LPARAM)0);

		Edit_SetReadOnly(info.encodeResEdit, true);
		Edit_SetReadOnly(info.decodeResEdit, true);

		ComputeEncode();
		ComputeDecode();

		return 0;
	}

	LRESULT Update(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
		if (uMsg == WM_COMMAND) {
			if (HIWORD(wParam) == EN_CHANGE) {
				if (info.encodeEdit == (HWND)lParam) {
					ComputeEncode();
				}
				else if (info.decodeEdit == (HWND)lParam) {
					ComputeDecode();
				}
			}
		}
		else if (uMsg == WM_CTLCOLORSTATIC) {
			if (lParam == (LPARAM)info.encodeEdit
				|| lParam == (LPARAM)info.encodeResEdit
				|| lParam == (LPARAM)info.decodeEdit
				|| lParam == (LPARAM)info.decodeResEdit
				|| lParam == (LPARAM)info.encodeEditLabel
				|| lParam == (LPARAM)info.decodeEditLabel
				|| lParam == (LPARAM)info.titleLabel
				) {
				return 0;
			}
		}
		return 1;
	}
	void Resize(int width, int height) {

		int y{ height / 2 - 28 * 3 };
		SetWindowPos(info.titleLabel, NULL, 0, y - 68, width, 60, SWP_SHOWWINDOW);

		SetWindowPos(info.encodeEdit, NULL, width / 2 - 250, y, 250, 24, SWP_SHOWWINDOW);
		SetWindowPos(info.encodeResEdit, NULL, width / 2 + 4, y, 250, 24, SWP_SHOWWINDOW);
		SetWindowPos(info.encodeEditLabel, NULL, 0, y, width / 2 - 250, 24, SWP_SHOWWINDOW);
		y += 28;
		SetWindowPos(info.decodeEdit, NULL, width / 2 - 250, y, 250, 24, SWP_SHOWWINDOW);
		SetWindowPos(info.decodeResEdit, NULL, width / 2 + 4, y, 250, 24, SWP_SHOWWINDOW);
		SetWindowPos(info.decodeEditLabel, NULL, 0, y, width / 2 - 250, 24, SWP_SHOWWINDOW);
		y += 28;

		tool::ui::window().SetTitleFont(info.titleLabel);
	}

	void errenc_nui() {
		tool::nui::NuiUseDefaultWindow dw{};
		static char encodeInput[0x100]{ 0 };
		static char encodeOutput[0x100]{ 0 };
		static char decodeInput[0x100]{ 0 };
		static char decodeOutput[0x100]{ 0 };
		static uint32_t decodeOutputVal{};
		static char exePath[MAX_PATH + 1]{ 0 };
		static std::string searchOutput{};
		static std::string notif{};

		static std::once_flag cfgof{};

		std::call_once(cfgof, [] {
			std::string exeCfg{ core::config::GetString("ui.error_coder.exe") };

			sprintf_s(exePath, exeCfg.c_str());
		});

		ImGui::SeparatorText("Error encoder");

		if (ImGui::InputText("Error code", encodeInput, sizeof(encodeInput))) {

			std::string encodeStr = encodeInput;

			if (encodeStr.empty()) {
				encodeOutput[0] = 0;
			}
			else {
				try {
					const char* cstr = encodeStr.c_str();
					char* end;
					uint32_t err = std::strtoul(cstr, &end, 10);
					if (end != cstr + encodeStr.length()) {
						throw std::runtime_error("Invalid code");
					}

					error_coder::ErrorCode code{};
					error_coder::Encode(code, err);

					sprintf_s(encodeOutput, "%s", error_coder::ToStr(code));
				}
				catch (std::exception& e) {
					sprintf_s(encodeOutput, "%s", e.what());
				}
			}
		}
		ImGui::InputText("Encoded", encodeOutput, sizeof(encodeOutput), ImGuiInputTextFlags_ReadOnly);

		ImGui::SeparatorText("Error decoder");

		if (ImGui::InputText("Error message", decodeInput, sizeof(decodeInput))) {
			std::string decodeStr = decodeInput;

			if (decodeStr.empty()) {
				decodeOutput[0] = 0;
				decodeOutputVal = 0;
				searchOutput = "";
			}
			else {

				std::string words[4]{};

				try {
					size_t idx{};
					for (int i = 0; i < 4; i++) {
						size_t f = decodeStr.find(' ', idx);
						if (f == std::string::npos) {
							if (i != 3) {
								throw std::runtime_error("Not enough components");
							}
							f = decodeStr.length();
						}
						if (f - idx == 0) {
							// empty word
							i--;
							idx = f + 1;
							continue;
						}

						words[i] = decodeStr.substr(idx, f - idx);
						idx = f + 1;
					}
					while (idx < decodeStr.length() && decodeStr[idx] == ' ') {
						idx++; // ignore end spaces
					}
					if (idx < decodeStr.length()) {
						throw std::runtime_error("Too many components");
					}

					error_coder::ErrorCode code{ words[0].c_str(), words[1].c_str(), words[2].c_str(), words[3].c_str() };

					uint32_t err = error_coder::Decode(code);
					sprintf_s(decodeOutput, "%lu", err);
					decodeOutputVal = err;
				}
				catch (std::exception& e) {
					sprintf_s(decodeOutput, "%s", e.what());
					decodeOutputVal = 0;
				}
			}
		}
		ImGui::InputText("Decoded", decodeOutput, sizeof(decodeOutput), ImGuiInputTextFlags_ReadOnly);

		ImGui::SeparatorText("Exe search");
		if (ImGui::InputText("Dump exe", exePath, sizeof(exePath))) {
			core::config::SetString("ui.error_coder.exe", exePath);
			tool::nui::SaveNextConfig();
		}
		if (ImGui::Button("Open file...")) {
			// Open file

			OPENFILENAMEW ofn;
			WCHAR szFile[MAX_PATH + 1] = { 0 };

			// Initialize OPENFILENAME
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = L"Dump file (.exe,.dll)\0*.exe;*.dll\0All\0*.*\0";
			ofn.lpstrTitle = L"Open Dump file";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if (GetOpenFileNameW(&ofn) == TRUE) {
				std::string path = utils::WStrToStr(ofn.lpstrFile);
				core::config::SetString("ui.error_coder.exe", path);
				snprintf(exePath, sizeof(exePath), "%s", path.data());
				tool::nui::SaveNextConfig();
			}
			searchOutput = "";
		}
		static hook::module_mapper::Module exe{ true };
		if (ImGui::Button("Load dump")) {
			exe.Free();
			if (!exe.Load(exePath)) {
				notif = std::format("Can't load {}", exePath);
			}
			else {
				notif = std::format("Loaded {}", exePath);
			}
			searchOutput = "";
		}


		if (exe && decodeOutputVal) {
			if (ImGui::Button("Search error")) {
				std::stringstream ss{};

				auto res{ exe->ScanNumber(decodeOutputVal) };
				if (res.size()) {
					for (auto& v : res) {
						ss << "0x" << std::hex << exe->Rloc(v.location) << " ";
						LOG_TRACE("Match one 0x{} -> {}", (void*)v.location, v.Get<uint32_t>());
					}
					notif = std::format("Find {} occurence(s)", res.size());
				}
				else {
					searchOutput = "";
					notif = "can't find error";
				}

				searchOutput = ss.str();
			}
			if (!searchOutput.empty()) {
				ImGui::InputText("Search", searchOutput.data(), searchOutput.length() + 1, ImGuiInputTextFlags_ReadOnly);
			}
		}

		if (!notif.empty()) {
			ImGui::Separator();

			ImGui::Text("%s", notif.data());
		}



	}
}
ADD_TOOL_UI(errenc, L"T8/9 Error encoder", Render, Update, Resize);
ADD_TOOL_NUI(errenc, "T8/9 Error", errenc_nui);

ADD_TOOL(errenc, "hash", "[error]", "encode an error", nullptr, errenc);
ADD_TOOL(errdec, "hash", "[w1] [w2] [w3] [w4]", "decode an error", nullptr, errdec);