#include <iostream>
#include <iomanip>
#include <cmath>

// Reproduce JSFX biquad_set function
void jsfx_biquad_highpass(double fc, double Q, double srate, double* coeffs)
{
    const double tau = 2.0 * 3.14159265358979323846;
    const double w = tau * fc / srate;
    const double cosw = std::cos(w);
    const double sinw = std::sin(w);
    const double alpha = sinw / (2.0 * Q);
    
    const double b0 = (1.0 + cosw) / 2.0;
    const double b1 = -(1.0 + cosw);
    const double b2 = (1.0 + cosw) / 2.0;
    const double a0 = 1.0 + alpha;
    const double a1 = -2.0 * cosw;
    const double a2 = 1.0 - alpha;
    
    const double inva0 = 1.0 / a0;
    coeffs[0] = b0 * inva0;
    coeffs[1] = b1 * inva0;
    coeffs[2] = b2 * inva0;
    coeffs[3] = a1 * inva0;
    coeffs[4] = a2 * inva0;
}

void jsfx_biquad_highshelf(double fc, double gdb, double srate, double* coeffs)
{
    const double tau = 2.0 * 3.14159265358979323846;
    const double w = tau * fc / srate;
    const double cosw = std::cos(w);
    const double sinw = std::sin(w);
    const double A = std::pow(10.0, gdb / 40.0);
    const double alpha = sinw / (2.0 * std::sqrt(2.0));
    const double two_sqrtA_alpha = 2.0 * std::sqrt(A) * alpha;
    
    const double b0 = A * ((A + 1.0) + (A - 1.0) * cosw + two_sqrtA_alpha);
    const double b1 = -2.0 * A * ((A - 1.0) + (A + 1.0) * cosw);
    const double b2 = A * ((A + 1.0) + (A - 1.0) * cosw - two_sqrtA_alpha);
    const double a0 = (A + 1.0) - (A - 1.0) * cosw + two_sqrtA_alpha;
    const double a1 = 2.0 * ((A - 1.0) - (A + 1.0) * cosw);
    const double a2 = (A + 1.0) - (A - 1.0) * cosw - two_sqrtA_alpha;
    
    const double inva0 = 1.0 / a0;
    coeffs[0] = b0 * inva0;
    coeffs[1] = b1 * inva0;
    coeffs[2] = b2 * inva0;
    coeffs[3] = a1 * inva0;
    coeffs[4] = a2 * inva0;
}

int main()
{
    const double srate = 48000.0;
    double hp[5], hs[5];
    
    jsfx_biquad_highpass(60.0, 0.5, srate, hp);
    jsfx_biquad_highshelf(4000.0, 4.0, srate, hs);
    
    std::cout << std::setprecision(17);
    std::cout << "Sample Rate: " << srate << " Hz\n\n";
    
    std::cout << "High-Pass (60 Hz, Q=0.5):\n";
    std::cout << "  b0 = " << hp[0] << "\n";
    std::cout << "  b1 = " << hp[1] << "\n";
    std::cout << "  b2 = " << hp[2] << "\n";
    std::cout << "  a1 = " << hp[3] << "\n";
    std::cout << "  a2 = " << hp[4] << "\n\n";
    
    std::cout << "High-Shelf (4000 Hz, +4 dB):\n";
    std::cout << "  b0 = " << hs[0] << "\n";
    std::cout << "  b1 = " << hs[1] << "\n";
    std::cout << "  b2 = " << hs[2] << "\n";
    std::cout << "  a1 = " << hs[3] << "\n";
    std::cout << "  a2 = " << hs[4] << "\n\n";
    
    std::cout << "Block size (400ms): " << static_cast<int>(0.4 * srate) << "\n";
    std::cout << "K_OFFSET_DB: -0.691\n";
    std::cout << "GATE_ABS_DB: -70.0\n";
    
    return 0;
}
