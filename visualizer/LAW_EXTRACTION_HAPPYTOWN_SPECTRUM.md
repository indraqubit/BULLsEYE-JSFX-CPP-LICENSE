# LAW EXTRACTION NOTE — happytown-s/Spectrum-Analyzer-Plugin

Repo referensi:
- https://github.com/happytown-s/Spectrum-Analyzer-Plugin
- License: **MIT**
- Stack: C++ / JUCE

Tujuan dokumen ini: mengekstrak hukum/pattern yang bisa diadopsi ke BULLsEYE, beserta batasannya.

---

## 1) Ringkasan Teknis yang Teramati

- FFT pipeline menggunakan `fftOrder = 12` (`fftSize = 4096`).
- Audio thread mendorong sample ke FIFO internal.
- Saat block siap, data dipindah ke `fftData` dan diberi Hann window.
- Handoff audio→UI menggunakan `std::atomic<bool> nextFFTBlockReady`.
- UI side menggunakan timer `startTimerHz(60)` untuk consume + repaint.

---

## 2) Laws yang Bisa Diadopsi (Adopt)

## A1 — Windowed FFT Staging Law
Gunakan staging jelas:
1) kumpulkan sample,
2) apply window,
3) jalankan FFT,
4) map ke domain visual.

**Alasan adopsi:** sederhana, stabil, mudah diuji.

## A2 — UI Pull Rendering Law
UI melakukan rendering berdasarkan timer cadence (bukan didorong langsung audio thread).

**Alasan adopsi:** menjaga decoupling audio/UI dan menghindari back-pressure ke audio.

## A3 — Readiness Signaling Law
Gunakan sinyal readiness antar thread (flag/snapshot version) untuk sinkronisasi konsumsi frame analyzer.

**Alasan adopsi:** mencegah UI membaca data setengah jadi.

---

## 3) Laws yang Tidak Diadopsi Mentah (Reject / Upgrade)

## R1 — Shared Writable FFT Buffer + Single Flag (as-is)
Pattern ini workable untuk kasus sederhana, tapi kurang kuat saat modul visualizer bertambah banyak.

**Upgrade BULLsEYE:** gunakan **immutable snapshot transport** (SPSC ring buffer atau versioned snapshot swap), bukan hanya satu shared writable buffer dengan satu flag.

## R2 — Fixed 60 FPS Tanpa Budget Control
60 FPS bagus, tapi harus adaptif terhadap budget UI thread/host.

**Upgrade BULLsEYE:** target default 30 FPS (Tahap 1), opsional 60 FPS jika budget aman.

## R3 — Tidak Ada Contract Gate Formal
Referensi tidak otomatis mengikuti contract matrix BULLsEYE.

**Upgrade BULLsEYE:** setiap adopsi wajib lolos gate:
- realtime,
- thread ownership,
- lifecycle host,
- deterministic fallback,
- performance budget.

---

## 4) Mapping ke Dokumen BULLsEYE

- `visualizer/ARCHITECTURE.md`
  - cocok untuk L0/L1/L3 (extractor, transport, render modules).
- `visualizer/STAGE_1_PLAN.md`
  - cocok untuk komponen Spectrum Analyzer tahap awal.
- `docs/AGENT_OPERATIONAL_CHECKLIST.md`
  - semua adopsi wajib sesuai red lines audio thread.

---

## 5) Keputusan Adopsi (Current)

**Adopt now (conceptual):**
- windowed FFT staging,
- timer-driven UI pull,
- readiness signaling antar thread.

**Adopt with modification:**
- transport data analyzer → upgrade ke immutable snapshot.

**Do not copy as-is:**
- shared mutable buffer model untuk skala modular multi-module.

---

## 6) Next Action (Disarankan)

1. Definisikan `AudioFeatureFrame` + `VisualFrameSnapshot` final.
2. Implement transport layer snapshot (SPSC/atomic swap) di scaffold internal.
3. Integrasikan Spectrum module Tahap 1 dengan cadence 30 FPS default.
4. Ukur metrik minimum: UI CPU, frame pacing, dan dropout safety.
