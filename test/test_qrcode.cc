#include <iostream>

#include "rautils/barcode/qrcode.h"

using rayalto::utils::barcode::Qrcode;

int main(int argc, char const *argv[]) {
    Qrcode qr;
    qr.error_correction_level(Qrcode::EC::M);
    Qrcode::Result qr_result = qr.encode("hello world!");
    /* qr_result.reverse_color_in_ostream(true); */
    std::cout << qr_result << std::endl;
    return 0;
}
