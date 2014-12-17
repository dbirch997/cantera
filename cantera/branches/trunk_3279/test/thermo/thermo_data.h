namespace {

// 2-region NASA coefficients; Order is significantly different from the
// standard NASA format.
const double h2o_nasa_coeffs[] = {
    1000.0, -3.029372670E+04, -8.490322080E-01, 4.198640560E+00,
    -2.036434100E-03, 6.520402110E-06, -5.487970620E-09, 1.771978170E-12,
    -3.000429710E+04, 4.966770100E+00,  3.033992490E+00, 2.176918040E-03,
    -1.640725180E-07, -9.704198700E-11, 1.682009920E-14};
const double h2o_comp[] = {2.0, 1.0, 0.0};

const double h2_nasa_coeffs[] = {
    1000.0, -9.17935173E+02, 6.83010238E-01, 2.34433112E+00,
    7.98052075E-03, -1.94781510E-05, 2.01572094E-08, -7.37611761E-12,
    -9.50158922E+02, -3.20502331E+00, 3.33727920E+00, -4.94024731E-05,
    4.99456778E-07, -1.79566394E-10, 2.00255376E-14};
const double h2_comp[] = {2.0, 0.0, 0.0};

const double o2_nasa_coeffs[] = {
    1000.0, -1.063943560E+03, 3.657675730E+00, 3.782456360E+00,
    -2.996734160E-03, 9.847302010E-06, -9.681295090E-09, 3.243728370E-12,
    -1.088457720E+03, 5.453231290E+00, 3.282537840E+00, 1.483087540E-03,
    -7.579666690E-07, 2.094705550E-10, -2.167177940E-14};
const double o2_comp[] = {0.0, 2.0, 0.0};

const double oh_nasa_coeffs[] = {
    1000.0, 3.615080560E+03, -1.039254580E-01, 3.992015430E+00,
    -2.401317520E-03, 4.617938410E-06, -3.881133330E-09, 1.364114700E-12,
    3.858657000E+03, 4.476696100E+00, 3.092887670E+00, 5.484297160E-04,
    1.265052280E-07, -8.794615560E-11, 1.174123760E-14};
const double oh_comp[] = {1.0, 1.0, 0.0};

// 2-region Shomate coefficients (from NIST Chemistry WebBook)
const double co2_shomate_coeffs[] = {
    1200.0, 24.99735, 55.18696, -33.69137, 7.948387, -0.136638, -403.6075, 228.2431,
    58.16639, 2.720074, -0.492289, 0.038844, -6.447293, -425.9186, 263.6125};
const double co2_comp[] = {0.0, 2.0, 1.0};

const double co_shomate_coeffs[] = {
    1300.0, 25.56759, 6.096130, 4.054656, -2.671301, 0.131021, -118.0089, 227.3665,
    35.15070, 1.300095, -0.205921, 0.013550, -3.282780, -127.8375, 231.7120};
const double co_comp[] = {0.0, 1.0, 1.0};

}
