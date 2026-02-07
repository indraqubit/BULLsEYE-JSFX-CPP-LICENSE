# VISUALIZER STAGE 1 PLAN

Dokumen ini mendefinisikan scope **Tahap 1** visualizer BULLsEYE.
Fokus: fitur inti yang user langsung rasakan, dengan risiko rendah terhadap audio engine.

---

## 1) Tujuan Tahap 1

Menyediakan visual feedback dasar yang useful saat playback:
- representasi spektrum frekuensi realtime,
- panel loudness inti (LUFS/True Peak),
- perilaku UI stabil tanpa mengganggu audio realtime.

---

## 2) User-Facing Components (Tahap 1)

## A. Spectrum Analyzer (Core)
- Kurva frekuensi realtime (log scale).
- Range dB terdefinisi (mis. floor/ceiling).
- Smoothing ringan agar terbaca, tidak “bergetar liar”.
- Peak hold basic (opsional ringan, bukan prioritas polish).

## B. Loudness Meter Panel (Core)
- Menampilkan metrik minimum:
  - LUFS Integrated (I)
  - LUFS Short-Term (ST)
  - True Peak
- Menyertakan target marker/zone sederhana untuk orientasi cepat user.

---

## 3) Arsitektur Minimal Tahap 1

1. **Audio thread** publish fitur analyzer (lock-free/snapshot).
2. **UI thread** consume snapshot via timer (target awal 30 Hz).
3. Komponen render hanya membaca snapshot immutable.

Ownership awal:
- Container utama visualizer: by-value (selalu ada).
- Modul yang siap opsional untuk tahap lanjut: bisa disiapkan memakai `std::unique_ptr`.

---

## 4) Contract Requirements (Wajib)

- Tidak ada lock/heap alloc/disk I-O/network di `processBlock()`.
- Snapshot analyzer immutable per audio block.
- UI tidak melakukan operasi yang back-pressure audio thread.
- Jika data analyzer tidak tersedia/stale, fallback visual konsisten (no glitch random).

---

## 5) Non-Goals Tahap 1

Item berikut **belum** masuk Tahap 1:
- spectrogram penuh,
- phase scope lanjutan,
- GPU rendering path,
- tema visual kompleks/polish tingkat lanjut,
- mode-switching visualizer multi-profil penuh.

---

## 6) Deliverables

- Dokumen arsitektur: `visualizer/ARCHITECTURE.md` ✅
- Dokumen intro: `visualizer/INTRODUCTION.md` ✅
- Dokumen tahap ini: `visualizer/STAGE_1_PLAN.md` ✅

Implementasi kode Tahap 1 dilakukan setelah review scope dokumen ini.

---

## 7) Acceptance Criteria Tahap 1

- Build bersih tanpa regression.
- Audio playback tetap stabil (no glitch/dropout akibat visualizer).
- UI analyzer + loudness panel tampil konsisten saat playback aktif.
- Performa UI masuk budget yang disepakati (CPU UI thread + frame pacing).

---

## 8) Exit Gate ke Tahap 2

Tahap 2 baru dimulai jika semua terpenuhi:
- Contract realtime tetap aman.
- Tidak ada issue lifecycle host kritikal.
- Data path analyzer terbukti stabil di uji playback normal.

---

## 9) Cost Classification (Stage 1 Scope)

## Low Cost (diprioritaskan)
- Timer UI 30 Hz default.
- Snapshot readiness sederhana (atomic/version check).
- Komponen container statis by-value.

## Medium Cost (masuk dengan kontrol)
- Log-frequency remap untuk spectrum.
- Smoothing ringan + peak hold basic.
- Overlay marker/grid secukupnya.

## High Cost (ditunda dari Stage 1)
- 60 FPS konstan sebagai default.
- Efek visual berat (blur/glow multi-pass).
- Repaint full-window terus-menerus.
- Spectrogram history buffer panjang.

Keputusan Stage 1:
- Implementasi fokus pada **Low + Medium**.
- Item **High** hanya boleh masuk setelah evidence perf gate.
