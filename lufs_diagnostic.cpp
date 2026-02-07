#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>

// Constants matching both implementations
const double TAU = 2.0 * 3.14159265358979323846;
const double LOG10 = 2.30258509299404568402;
const double K_OFFSET_DB = -0.691;
const double NEG_INF = -1000000000.0;
const double NEG_INF_THR = -900000000.0;
const double GATE_ABS = -70.0;
const double EPS = 1e-12;

// Filter state
struct FilterState {
    double x1 = 0, x2 = 0, y1 = 0, y2 = 0;
};

struct BiquadCoeffs {
    double b0, b1, b2, a1, a2;
};

// JSFX-style biquad coefficient calculation
BiquadCoeffs calcHighPass(double fc, double Q, double srate) {
    double w = TAU * fc / srate;
    double cosw = std::cos(w);
    double sinw = std::sin(w);
    double alpha = sinw / (2.0 * Q);
    
    double b0 = (1.0 + cosw) / 2.0;
    double b1 = -(1.0 + cosw);
    double b2 = (1.0 + cosw) / 2.0;
    double a0 = 1.0 + alpha;
    double a1 = -2.0 * cosw;
    double a2 = 1.0 - alpha;
    
    return {b0/a0, b1/a0, b2/a0, a1/a0, a2/a0};
}

BiquadCoeffs calcHighShelf(double fc, double gdb, double srate) {
    double w = TAU * fc / srate;
    double cosw = std::cos(w);
    double sinw = std::sin(w);
    double A = std::pow(10.0, gdb / 40.0);
    double alpha = sinw / (2.0 * std::sqrt(2.0));  // hardcoded sqrt(2)
    double two_sqrtA_alpha = 2.0 * std::sqrt(A) * alpha;
    
    double b0 = A * ((A+1) + (A-1)*cosw + two_sqrtA_alpha);
    double b1 = -2.0 * A * ((A-1) + (A+1)*cosw);
    double b2 = A * ((A+1) + (A-1)*cosw - two_sqrtA_alpha);
    double a0 = (A+1) - (A-1)*cosw + two_sqrtA_alpha;
    double a1 = 2.0 * ((A-1) - (A+1)*cosw);
    double a2 = (A+1) - (A-1)*cosw - two_sqrtA_alpha;
    
    return {b0/a0, b1/a0, b2/a0, a1/a0, a2/a0};
}

double applyBiquad(double x, const BiquadCoeffs& c, FilterState& s) {
    double y = c.b0*x + c.b1*s.x1 + c.b2*s.x2 - c.a1*s.y1 - c.a2*s.y2;
    s.x2 = s.x1; s.x1 = x;
    s.y2 = s.y1; s.y1 = y;
    return y;
}

// Simulate JSFX style measurement
double measureLUFS_JSFX_Style(const std::vector<double>& samples, double srate) {
    // Filter setup
    BiquadCoeffs hp = calcHighPass(60.0, 0.5, srate);
    BiquadCoeffs hs = calcHighShelf(4000.0, 4.0, srate);
    FilterState hpL, hsL, hpR, hsR;
    
    // Block accumulation
    int blkN = std::max(1, (int)std::floor(0.400 * srate));
    double blk_sum = 0;
    int blk_cnt = 0;
    double int_energy_sum = 0;
    int int_sample_sum = 0;
    double L_int = NEG_INF;
    
    int blocksProcessed = 0;
    int blocksGated = 0;
    
    std::cout << "JSFX-Style Simulation:\n";
    std::cout << "  Block size: " << blkN << " samples\n";
    
    for (size_t i = 0; i < samples.size(); i += 2) {
        double spl_l = samples[i];
        double spl_r = (i+1 < samples.size()) ? samples[i+1] : 0.0;
        
        // K-weight
        double yl = applyBiquad(applyBiquad(spl_l, hp, hpL), hs, hsL);
        double yr = applyBiquad(applyBiquad(spl_r, hp, hpR), hs, hsR);
        
        // Energy
        double e = yl*yl + yr*yr;
        
        // Block accumulation
        blk_sum += e;
        blk_cnt += 1;
        
        if (blk_cnt >= blkN) {
            double blk_mean = blk_sum / blk_cnt;
            double blk_lufs = (blk_mean > 0) ? (K_OFFSET_DB + 10.0 * std::log10(blk_mean)) : NEG_INF;
            
            double gate_rel = (L_int <= NEG_INF_THR) ? GATE_ABS : (L_int - 10.0);
            double gate_thr = std::max(GATE_ABS, gate_rel);
            
            blocksProcessed++;
            
            if (blk_lufs >= gate_thr) {
                int_energy_sum += blk_sum;
                int_sample_sum += blk_cnt;
                blocksGated++;
            }
            
            blk_sum = 0;
            blk_cnt = 0;
            
            if (int_sample_sum > 0) {
                double mean_all = int_energy_sum / int_sample_sum;
                L_int = K_OFFSET_DB + 10.0 * std::log10(std::max(mean_all, EPS));
            } else {
                L_int = NEG_INF;
            }
            
            if (blocksProcessed <= 5) {
                std::cout << "  Block " << blocksProcessed << ": blk_lufs=" << std::fixed << std::setprecision(2) << blk_lufs 
                          << " gate_thr=" << gate_thr << " passed=" << (blk_lufs >= gate_thr ? "YES" : "NO")
                          << " L_int=" << L_int << "\n";
            }
        }
    }
    
    std::cout << "  Total blocks: " << blocksProcessed << ", Gated blocks: " << blocksGated << "\n";
    std::cout << "  Final int_energy_sum: " << int_energy_sum << "\n";
    std::cout << "  Final int_sample_sum: " << int_sample_sum << "\n";
    std::cout << "  Final L_int: " << L_int << " LUFS\n\n";
    
    return L_int;
}

// Simulate C++ style measurement (with the bug fix)
double measureLUFS_CPP_Style(const std::vector<double>& samples, double srate) {
    // Filter setup (identical)
    BiquadCoeffs hp = calcHighPass(60.0, 0.5, srate);
    BiquadCoeffs hs = calcHighShelf(4000.0, 4.0, srate);
    FilterState hpL, hsL, hpR, hsR;
    
    // Block accumulation
    int blockSize = (int)((400.0 / 1000.0) * srate);
    double blockAccumulator = 0;
    int blockCount = 0;
    double energySum = 0;
    int sampleSum = 0;
    double integratedLUFS = -120.0;  // C++ initializes to MIN_DISPLAY_DB, not NEG_INF
    
    int blocksProcessed = 0;
    int blocksGated = 0;
    
    std::cout << "C++-Style Simulation (FIXED):\n";
    std::cout << "  Block size: " << blockSize << " samples\n";
    
    for (size_t i = 0; i < samples.size(); i += 2) {
        double spl_l = samples[i];
        double spl_r = (i+1 < samples.size()) ? samples[i+1] : 0.0;
        
        // K-weight (identical)
        double yl = applyBiquad(applyBiquad(spl_l, hp, hpL), hs, hsL);
        double yr = applyBiquad(applyBiquad(spl_r, hp, hpR), hs, hsR);
        
        // Energy (identical)
        double energy = yl*yl + yr*yr;
        
        // Block accumulation
        blockAccumulator += energy;
        blockCount++;
        
        if (blockCount >= blockSize && blockSize > 0) {
            double blockMean = blockAccumulator / blockCount;
            double blockLUFS = (blockMean > 1e-18) ? (K_OFFSET_DB + 10.0 * std::log10(blockMean)) : -120.0;
            
            // FIXED: Use NEG_INF_THR check like JSFX
            double gateThr;
            if (integratedLUFS <= NEG_INF_THR) {  // -900000000
                gateThr = GATE_ABS;
            } else {
                double gateRel = integratedLUFS - 10.0;
                gateThr = std::max(GATE_ABS, gateRel);
            }
            
            blocksProcessed++;
            
            if (blockLUFS >= gateThr) {
                energySum += blockAccumulator;
                sampleSum += blockCount;
                blocksGated++;
            }
            
            blockAccumulator = 0;
            blockCount = 0;
            
            if (sampleSum > 0) {
                double meanAll = energySum / sampleSum;
                integratedLUFS = K_OFFSET_DB + 10.0 * std::log10(std::max(meanAll, 1e-18));
            }
            
            if (blocksProcessed <= 5) {
                std::cout << "  Block " << blocksProcessed << ": blk_lufs=" << std::fixed << std::setprecision(2) << blockLUFS 
                          << " gate_thr=" << gateThr << " passed=" << (blockLUFS >= gateThr ? "YES" : "NO")
                          << " L_int=" << integratedLUFS << "\n";
            }
        }
    }
    
    std::cout << "  Total blocks: " << blocksProcessed << ", Gated blocks: " << blocksGated << "\n";
    std::cout << "  Final energySum: " << energySum << "\n";
    std::cout << "  Final sampleSum: " << sampleSum << "\n";
    std::cout << "  Final integratedLUFS: " << integratedLUFS << " LUFS\n\n";
    
    return integratedLUFS;
}

// Simulate OLD C++ style (with the bug)
double measureLUFS_CPP_OLD_Style(const std::vector<double>& samples, double srate) {
    BiquadCoeffs hp = calcHighPass(60.0, 0.5, srate);
    BiquadCoeffs hs = calcHighShelf(4000.0, 4.0, srate);
    FilterState hpL, hsL, hpR, hsR;
    
    int blockSize = (int)((400.0 / 1000.0) * srate);
    double blockAccumulator = 0;
    int blockCount = 0;
    double energySum = 0;
    int sampleSum = 0;
    double integratedLUFS = -120.0;
    
    int blocksProcessed = 0;
    int blocksGated = 0;
    
    std::cout << "C++-Style Simulation (OLD BUG):\n";
    std::cout << "  Block size: " << blockSize << " samples\n";
    
    for (size_t i = 0; i < samples.size(); i += 2) {
        double spl_l = samples[i];
        double spl_r = (i+1 < samples.size()) ? samples[i+1] : 0.0;
        
        double yl = applyBiquad(applyBiquad(spl_l, hp, hpL), hs, hsL);
        double yr = applyBiquad(applyBiquad(spl_r, hp, hpR), hs, hsR);
        double energy = yl*yl + yr*yr;
        
        blockAccumulator += energy;
        blockCount++;
        
        if (blockCount >= blockSize && blockSize > 0) {
            double blockMean = blockAccumulator / blockCount;
            double blockLUFS = (blockMean > 1e-18) ? (K_OFFSET_DB + 10.0 * std::log10(blockMean)) : -120.0;
            
            // OLD BUG: Used -119 threshold instead of NEG_INF_THR
            double gateThr;
            if (integratedLUFS <= -119.0) {  // BUG: MIN_DISPLAY_DB + 1
                gateThr = GATE_ABS;
            } else {
                double gateRel = integratedLUFS - 10.0;
                gateThr = std::max(GATE_ABS, gateRel);
            }
            
            blocksProcessed++;
            
            if (blockLUFS >= gateThr) {
                energySum += blockAccumulator;
                sampleSum += blockCount;
                blocksGated++;
            }
            
            blockAccumulator = 0;
            blockCount = 0;
            
            if (sampleSum > 0) {
                double meanAll = energySum / sampleSum;
                integratedLUFS = K_OFFSET_DB + 10.0 * std::log10(std::max(meanAll, 1e-18));
            }
            
            if (blocksProcessed <= 5) {
                std::cout << "  Block " << blocksProcessed << ": blk_lufs=" << std::fixed << std::setprecision(2) << blockLUFS 
                          << " gate_thr=" << gateThr << " passed=" << (blockLUFS >= gateThr ? "YES" : "NO")
                          << " L_int=" << integratedLUFS << "\n";
            }
        }
    }
    
    std::cout << "  Total blocks: " << blocksProcessed << ", Gated blocks: " << blocksGated << "\n";
    std::cout << "  Final energySum: " << energySum << "\n";
    std::cout << "  Final sampleSum: " << sampleSum << "\n";
    std::cout << "  Final integratedLUFS: " << integratedLUFS << " LUFS\n\n";
    
    return integratedLUFS;
}

int main() {
    double srate = 48000.0;
    
    // Generate 5 seconds of stereo sine wave at -10 dBFS
    // This simulates typical music-level audio
    int duration_samples = 5 * 48000 * 2;  // 5 sec * 48kHz * stereo
    std::vector<double> samples(duration_samples);
    
    double amplitude = std::pow(10.0, -10.0 / 20.0);  // -10 dBFS
    double freq = 1000.0;  // 1kHz
    
    for (int i = 0; i < duration_samples; i += 2) {
        double t = (i/2) / srate;
        double val = amplitude * std::sin(2.0 * 3.14159265 * freq * t);
        samples[i] = val;      // L
        samples[i+1] = val;    // R
    }
    
    std::cout << "=== LUFS Measurement Comparison ===\n";
    std::cout << "Test signal: 1kHz sine, -10 dBFS, 5 seconds, stereo\n";
    std::cout << "Sample rate: " << srate << " Hz\n\n";
    
    double jsfx_lufs = measureLUFS_JSFX_Style(samples, srate);
    double cpp_fixed_lufs = measureLUFS_CPP_Style(samples, srate);
    double cpp_old_lufs = measureLUFS_CPP_OLD_Style(samples, srate);
    
    std::cout << "=== RESULTS ===\n";
    std::cout << "JSFX:        " << std::fixed << std::setprecision(2) << jsfx_lufs << " LUFS\n";
    std::cout << "C++ (FIXED): " << cpp_fixed_lufs << " LUFS\n";
    std::cout << "C++ (OLD):   " << cpp_old_lufs << " LUFS\n";
    std::cout << "\n";
    std::cout << "JSFX vs C++ FIXED diff: " << std::abs(jsfx_lufs - cpp_fixed_lufs) << " dB\n";
    std::cout << "JSFX vs C++ OLD diff:   " << std::abs(jsfx_lufs - cpp_old_lufs) << " dB\n";
    
    return 0;
}
