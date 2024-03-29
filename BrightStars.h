
// SBO database

struct BSTAR {
  char con[4];
  uint8_t hr;
  uint8_t mn;
  int8_t  dd;
  uint8_t dm;
  char sname[20];
};

// bright stars and now other objects
#define NUMSTAR 225
const struct BSTAR bstar[NUMSTAR] = {
{"And", 00, 39, + 30, 51,"delta And * "},
{"Psc", 00, 39, + 21, 26,"55 Psc ** "},
{"And", 00, 42, + 40, 52,"M32 Ell Gal "},
{"And", 00, 42, + 41, 16,"M31 Andromeda S"},
{"Scl", 00, 47, - 25, 17,"NGC 253 Spl Gal"},
{"Cas", 00, 49, + 57, 49,"eta Cas ** "},
{"And", 01, 9, + 35, 37,"beta And * "},
{"Cas", 01, 33, + 60, 42,"M 103 Open Cl "},
{"Tri", 01, 33, + 30, 39,"M 33 Pinwheel S"},
{"Psc", 01, 36, + 15, 47,"M 74 Spl Gal "},
{"Per", 01, 41, + 51, 34,"M 76 Barbell Pl"},
{"Ari", 01, 53, + 19, 17,"gamma Ari ** "},
{"Psc", 02, 02, + 02, 45,"alpha Psc ** "},
{"And", 02, 03, + 42, 19,"gamma And * ** "},
{"Tri", 02, 12, + 30, 18,"6 Tri ** "},
{"Cet", 02, 12, -2, 23,"66 Cet * * "},
{"Per", 02, 19, + 57, 8,"NGC 869 h Ope"},
{"Per", 02, 22, + 57, 06,"NGC 884 chi O"},
{"Cas", 02, 29, + 67, 24,"iota Cas *** "},
{"Per", 02, 42, + 42, 46,"M 34 Open Cl "},
{"Cet", 02, 42, 0, 01,"M 77 Spl Gal "},
{"Cet", 02, 43, + 03, 14,"gamma Cet ** "},
{"Per", 02, 50, + 55, 53,"eta Per * * "},
{"Eri", 02, 58, - 40, 18,"theta Eri ** "},
{"Cep", 03, 06, + 79, 25,"Struve 320 ** "},
{"Per", 03, 24, + 49, 51,"alpha Per * "},
{"Tau", 03, 47, + 24, 06,"M 45 Pleiades O"},
{"Eri", 03, 54, -2, 57,"32 Eri ** "},
{"Cam", 03, 57, + 61, 06,"Struve 67 ** "},
{"Eri", 04, 14, - 12, 44,"NGC 1535 Plt Ne"},
{"Tau", 04, 35, + 16, 30,"Aldebaran * "},
{"Aur", 04, 57, + 33, 10,"iota Aur * "},
{"Ori", 05, 14, -8, 12,"Rigel ** "},
{"Aur", 05, 16, + 49, 59,"Capella * "},
{"Lep", 05, 24, - 24, 31,"M 79 Glob Cl "},
{"Ori", 05, 24, -2, 23,"eta Ori ** "},
{"Aur", 05, 28, + 35, 50,"M 38 Open Cl "},
{"Tau", 05, 34, + 22, 01,"Crab Nebula Spr"},
{"Ori", 05, 35, + 9, 56,"lambda Ori ** "},
{"Ori", 05, 35, -5, 23,"Trapezium **** "},
{"Ori", 05, 35, -5, 23,"M 42 Orion Neb."},
{"Ori", 05, 35, -5, 54,"iota Ori ** "},
{"Ori", 05, 35, -5, 16,"M 43 NE Ori Neb"},
{"Aur", 05, 36, + 34, 8,"M 36 Open Cl "},
{"Ori", 05, 38, -2, 36,"sigma Ori *** *"},
{"Ori", 05, 40, -1, 56,"zeta Ori ** * "},
{"Ori", 05, 46, + 00, 03,"M 78 Dif Neb "},
{"Aur", 05, 52, + 32, 32,"M37 Open Cl "},
{"Ori", 05, 55, + 07, 24,"Betelgeuse * "},
{"Aur", 05, 59, + 44, 56,"beta Aur * "},
{"Aur", 05, 59, + 37, 12,"theta Aur ** "},
{"Gem", 06, 8, + 24, 20,"M 35 Open Cl "},
{"Mon", 06, 23, + 04, 35,"epsilon Mon * *"},
{"Mon", 06, 28, -7, 02,"beta Mon *** "},
{"Aur", 06, 36, + 38, 26,"UU Aur * (red) "},
{"CMa", 06, 45, - 16, 43,"Sirius ** "},
{"Lyn", 06, 46, + 59, 26,"12 Lyn *** "},
{"CMa", 06, 47, - 20, 45,"M41 Open Cl "},
{"Gem", 06, 52, + 33, 57,"theta Gem * "},
{"CMa", 06, 56, - 14, 02,"mu CMa ** "},
{"Mon", 07, 02, -8, 20,"M 50 Open Cl "},
{"Gem", 07, 20, + 21, 59,"delta Gem ** "},
{"Gem", 07, 29, + 20, 54,"NGC2392 Eskimo "},
{"Gem", 07, 34, + 31, 53,"Castor ** * "},
{"Pup", 07, 36, - 14, 28,"M 47 Open Cl "},
{"Pup", 07, 38, - 26, 48,"kappa Pup ** "},
{"CMi", 07, 39, + 05, 13,"Procyon * "},
{"Pup", 07, 41, - 14, 49,"M 46 Open Cl "},
{"Pup", 07, 44, - 23, 52,"M 93 Open Cl "},
{"Gem", 07, 45, + 28, 01,"Pollux * "},
{"Cnc", 8, 12, + 17, 38,"zeta Cnc *** "},
{"Hya", 8, 13, -5, 47,"M 48 Open Cl "},
{"Cnc", 8, 40, + 19, 59,"M 44 Praesepe O"},
{"Cnc", 8, 46, + 28, 45,"iota Cnc * * "},
{"Cnc", 8, 51, + 11, 48,"M 67 Open Cl "},
{"UMa", 8, 59, + 48, 02,"iota UMa ** "},
{"Lyn", 9, 21, + 34, 23,"alpha Lyn * "},
{"UMa", 9, 55, + 69, 03,"M 81 Spl Gal "},
{"UMa", 9, 55, + 69, 40,"M 82 Irr Gal "},
{"Leo", 10, 19, + 19, 50,"gamma Leo ** "},
{"UMa", 10, 22, + 41, 30,"mu UMa * "},
{"Hya", 10, 24, - 18, 38,"NGC 3242 Plt Ne"},
{"Leo", 10, 43, + 11, 42,"M 95 BSp Gal "},
{"Leo", 10, 46, + 11, 49,"M 96 Spl Gal "},
{"Leo", 10, 47, + 12, 35,"M 105 Ell Gal "},
{"UMa", 11, 9, + 44, 29,"psi UMa * "},
{"UMa", 11, 11, + 55, 40,"M 108 Spl Gal "},
{"UMa", 11, 14, + 55, 01,"M 97 Owl Neb Pl"},
{"UMa", 11, 18, + 31, 31,"xi UMa ** "},
{"Leo", 11, 18, + 13, 05,"M 65 Spl Gal "},
{"Leo", 11, 20, + 12, 59,"M 66 Spl Gal "},
{"UMa", 11, 57, + 53, 22,"M109 BSp Gal "},
{"Com", 12, 13, + 14, 54,"M 98 Spl Gal "},
{"Com", 12, 18, + 14, 25,"M 99 Spl Gal "},
{"CVn", 12, 18, + 47, 18,"M106 Spl Gal "},
{"Vir", 12, 21, + 04, 28,"M 61 Spl Gal "},
{"UMa", 12, 22, + 58, 05,"M 40 ** "},
{"Com", 12, 22, + 15, 49,"M 100 Spl Gal "},
{"Vir", 12, 25, + 12, 53,"M 84 Ell Gal "},
{"Com", 12, 25, + 18, 11,"M 85 Ell Gal "},
{"Vir", 12, 26, + 12, 56,"M 86 Ell Gal "},
{"Vir", 12, 29, + 02, 03,"3C273 Quasar "},
{"Vir", 12, 29, + 8, 00,"M 49 Ell Gal "},
{"Crv", 12, 29, - 16, 30,"delta Crv * * "},
{"Vir", 12, 30, + 12, 23,"M 87 Ell Gal "},
{"Com", 12, 32, + 14, 25,"M 88 Spl Gal "},
{"Com", 12, 35, + 18, 22,"24 Com * * "},
{"Com", 12, 35, + 14, 29,"M 91 BSp Gal "},
{"Vir", 12, 35, + 12, 33,"M 89 Ell Gal "},
{"Vir", 12, 36, + 13, 9,"M 90 Spl Gal "},
{"Vir", 12, 37, + 11, 49,"M 58 Spl Gal "},
{"Hya", 12, 39, - 26, 45,"M 68 Glob Cl "},
{"Vir", 12, 39, - 11, 37,"M104 Sombrero S"},
{"Vir", 12, 41, -1, 27,"gamma Vir ** "},
{"Vir", 12, 42, + 11, 38,"M 59 Ell Gal "},
{"Vir", 12, 43, + 11, 33,"M 60 Ell Gal "},
{"CVn", 12, 45, + 45, 26,"Y Cvn * (red) "},
{"Cam", 12, 49, + 83, 24,"Struve 1694 * *"},
{"CVn", 12, 50, + 41, 07,"M 94 Spl Gal "},
{"CVn", 12, 56, + 38, 19,"Cor Caroli * * "},
{"Com", 12, 56, + 21, 40,"M 64 Black Eye "},
{"Com", 13, 12, + 18, 10,"M 53 Glob Cl "},
{"CVn", 13, 15, + 42, 02,"M 63 Sunflower "},
{"UMa", 13, 23, + 54, 55,"Mizar (w/Alcor)"},
{"Vir", 13, 25, - 11, 9,"Spica * "},
{"CVn", 13, 29, + 47, 11,"M 51 Whirlpool "},
{"Hya", 13, 37, - 29, 51,"M 83 Spl Gal "},
{"CVn", 13, 43, 28, 22, "M 3 Glob C1 "},
{"UMa", 13, 47, 49, 18,"eta Uma * "},
{"UMa", 14, 03, + 54, 21,"M 101 Spl Gal "},
{"UMa", 14, 03, + 54, 21,"M 102 = M 101 S"},
{"Boo", 14, 15, + 19, 11,"Arcturus * "},
{"Boo", 14, 32, + 38, 18,"gamma Boo ** "},
{"Boo", 14, 44, + 27, 04,"epsilon Boo ** "},
{"Boo", 14, 51, + 19, 06,"xi Boo ** "},
{"Boo", 15, 15, + 33, 18,"delta Boo * * "},
{"Ser", 15, 18, 2 , 5, "M 5 Glob C1 "},
{"Boo", 15, 24, + 37, 21,"mu Boo * ** "},
{"Ser", 15, 34, + 10, 32,"delta Ser ** "},
{"CrB", 15, 39, + 36, 38,"zeta CrB ** "},
{"Sco", 16, 04, - 11, 22,"xi Sco *** ** "},
{"Sco", 16, 05, - 19, 48,"beta Sco * * "},
{"Sco", 16, 11, - 19, 27,"nu Sco ** ** "},
{"Sco", 16, 17, - 22, 59,"M 80 Glob Cl "},
{"Her", 16, 19, + 46, 18,"tau Her * "},
{"Sco", 16, 23, - 26, 30,"M 4 Glob Cl "},
{"Sco", 16, 29, - 26, 25,"Antares ** "},
{"Oph", 16, 32, - 13, 02,"M 107 Glob Cl "},
{"Dra", 16, 36, + 52, 54,"16  17 Dra * **"},
{"Her", 16, 41, + 36, 27,"M 13 Hercules G"},
{"Her", 16, 44, + 23, 47,"NGC 6210 Plt Ne"},
{"Oph", 16, 47, -1, 57,"M 12 Glob Cl "},
{"Oph", 16, 57, -4, 06,"M 10 Glob Cl "},
{"Oph", 17, 01, - 30, 07,"M 62 Glob Cl "},
{"Oph", 17, 02, - 26, 15,"M 19 Glob Cl "},
{"Her", 17, 14, + 14, 23,"alpha Her ** "},
{"Her", 17, 15, + 24, 50,"delta Her ** "},
{"Her", 17, 15, + 36, 48,"pi Her * "},
{"Her", 17, 17, + 43, 8,"M 92 Glob Cl "},
{"Oph", 17, 18, - 24, 17,"omicron Oph ** "},
{"Oph", 17, 19, - 18, 31,"M 9 Glob Cl "},
{"Her", 17, 23, + 37, 8,"rho Her ** "},
{"Dra", 17, 32, + 55, 10,"nu Dra * * "},
{"Oph", 17, 37, -3, 16,"M 14 Glob Cl "},
{"Sco", 17, 40, - 32, 12,"M 6 Butterfly O"},
{"Dra", 17, 41, + 72, 9,"psi Dra * * "},
{"Her", 17, 53, + 40, 00,"90 Her ** "},
{"Sco", 17, 53, - 34, 48,"M 7 Open Cl "},
{"Sgr", 17, 56, - 19, 01,"M 23 Open Cl "},
{"Dra", 17, 58, + 66, 38,"NGC 6543 Plt Ne"},
{"Her", 18, 01, + 21, 35,"95 Her ** "},
{"Sgr", 18, 02, - 23, 02,"M 20 Triffid Di"},
{"Sgr", 18, 03, - 24, 23,"M 8 Lagoon Neb "},
{"Sgr", 18, 04, - 22, 29,"M 21 Open Cl "},
{"Oph", 18, 05, + 02, 30,"70 Oph ** "},
{"Oph", 18, 12, + 06, 50,"NGC 6572 Plt Ne"},
{"Sgr", 18, 18, - 18, 24,"M 24 Open Cl "},
{"Ser", 18, 18, - 13, 46,"M 16 Eagle Neb."},
{"Sgr", 18, 19, - 17, 07,"M 18 Open Cl "},
{"Sgr", 18, 20, - 16, 10,"M 17 Omega Dif "},
{"Sgr", 18, 24, - 24, 52,"M 28 Glob Cl "},
{"Sgr", 18, 31, - 32, 20,"M 69 Glob Cl "},
{"Sgr", 18, 31, - 19, 14,"M 25 Open Cl "},
{"Sgr", 18, 36, - 23, 55,"M 22 Glob Cl "},
{"Lyr", 18, 36, + 38, 47,"503Vega * * "},
{"Sgr", 18, 43, - 32, 18,"M 70 Glob Cl "},
{"Lyr", 18, 44, + 39, 38,"epsilon Lyr ** "},
{"Sct", 18, 45, -9, 23,"M 26 Open Cl "},
{"Sct", 18, 51, -6, 16,"M 11 Open Cl "},
{"Lyr", 18, 53, + 33, 01,"M 57 Ring Neb P"},
{"Sgr", 18, 55, - 30, 28,"M 54 Glob Cl "},
{"Ser", 18, 56, + 04, 12,"theta Ser * * "},
{"Lyr", 19, 16, + 30, 10,"M 56 Glob Cl "},
{"Cyg", 19, 30, + 27, 57,"Albireo * * "},
{"Sgr", 19, 40, - 30, 56,"M 55 Glob Cl "},
{"Sgr", 19, 43, - 14, 8,"NGC 6818 Plt Ne"},
{"Cyg", 19, 44, + 50, 31,"NGC 6826 Plt Ne"},
{"Cyg", 19, 44, + 45, 07,"delta Cyg ** "},
{"Aql", 19, 50, + 8, 52,"Altair * "},
{"Sge", 19, 53, + 18, 46,"M 71 Glob Cl "},
{"Vul", 19, 59, + 22, 43,"M 27 Plt Neb "},
{"Sgr", 20, 06, - 21, 55,"M 75 Glob Cl "},
{"Cyg", 20, 23, + 38, 31,"M 29 Open Cl "},
{"Cyg", 20, 41, + 32, 18,"49 Cyg ** "},
{"Cyg", 20, 41, + 45, 16,"Deneb * "},
{"Del", 20, 46, + 16, 07,"gamma Del ** "},
{"Aqr", 20, 53, - 12, 32,"M 72 Glob Cl "},
{"Aqr", 20, 58, - 12, 37,"M 73 **** "},
{"Aqr", 21, 04, - 11, 22,"NGC7009 Saturn "},
{"Cyg", 21, 06, + 38, 44,"61 Cyg * * "},
{"Cyg", 21, 14, + 38, 02,"tau Cyg ** "},
{"Cep", 21, 28, + 70, 33,"beta Cep * * "},
{"Peg", 21, 30, + 12, 10,"M 15 Glob Cl "},
{"Cyg", 21, 32, + 48, 26,"M 39 Open Cl "},
{"Aqr", 21, 33, 0, 49,"M2 Glob Cl "},
{"Cap", 21, 40, - 23, 11,"M30 Glob Cl "},
{"Cep", 21, 43, + 58, 46,"mu Cep * (red)"},
{"Aqr", 22, 28, 0, 01,"zeta Aqr ** "},
{"Cep", 22, 29, + 58, 24,"delta Cep * * "},
{"Peg", 22, 43, + 30, 13,"eta Peg * * "},
{"Aqr", 23, 19, - 13, 27,"94 Aqr * * "},
{"Cas", 23, 24, + 61, 35,"M 52 Open Cl "},
{"And", 23, 25, + 42, 32,"NGC 7662 Plt Ne"},
{"And", 23, 37, + 46, 27,"lambda And * "},
{"Psc", 23, 46, + 03, 29,"19 (TX) Psc * ("},
};
