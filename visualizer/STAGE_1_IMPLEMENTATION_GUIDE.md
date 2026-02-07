# VISUALIZER STAGE 1 IMPLEMENTATION GUIDE

Dokumen ini menjelaskan **cara implementasi** Stage 1 visualizer BULLsEYE secara bertahap dan aman, tanpa langsung memaksa perubahan besar ke arsitektur plugin yang sudah stabil.

Dokumen ini adalah jembatan antara:
- `visualizer/STAGE_1_PLAN.md` (apa yang dibangun),
- `visualizer/ARCHITECTURE.md` (bagaimana arsitekturnya),
- baseline kode saat ini di `Source/` (kondisi nyata implementasi).

Status dokumen:
- Ini adalah **dokumen panduan implementasi** (bukan implementasi kode langsung).
- Seluruh rekomendasi disusun agar kompatibel dengan baseline plugin saat ini.

---

## 1) Scope Implementasi Stage 1

Fokus implementasi:
- **Spectrum Analyzer (core)**
- **Loudness panel (core)**

Loudness panel sudah ada di baseline melalui:
- `StatusDisplayComponent`
- `CircularMeterComponent`

Maka Stage 1 implementation utama adalah menambahkan jalur analyzer yang benar secara contract untuk Spectrum, sambil mempertahankan stabilitas metering existing.

---

## 2) Baseline Kode Saat Ini (Source Mapping)

## A. Processor side
- `Source/PluginProcessor.h`
- `Source/PluginProcessor.cpp`

Kondisi:
- Audio diproses per-sample via `dspCore.process(left, right)` di `processBlock()`.
- Parameter content type dipoll tiap block.
- Sudah ada akses metrik untuk UI (`getIntegratedLUFS()`, `getTruePeakDB()`, dll).

## B. Editor/UI side
- `Source/PluginEditor.h`
- `Source/PluginEditor.cpp`

Kondisi:
- UI update via `Timer`.
- Sudah ada komponen untuk mode selector + loudness visual.
- Cadence refresh mengikuti `UISSOT::Timing::UI_REFRESH_RATE_HZ` (saat ini **30 Hz**).

Implikasi:
- Pondasi timer-driven UI sudah sesuai law “UI pull rendering”.
- Yang belum formal: transport analyzer snapshot immutable lintas thread.

---

## 3) Target Arsitektur Stage 1 (Praktis)

Mengacu L0–L5 pada `ARCHITECTURE.md`, implementasi praktis Stage 1 diarahkan ke:

- **L0**: extractor data analyzer minimal dari jalur audio (ringan, deterministic)
- **L1**: transport snapshot thread-safe (audio publish-only, UI consume-only)
- **L2**: transform visual ringan (dB clamp, log remap bila perlu)
- **L3**: `SpectrumModule` render dasar
- **L5**: integrasi ke `PluginEditor` timer loop

L4 (orchestrator penuh) boleh ditunda; Stage 1 cukup 1 modul spectrum terintegrasi langsung selama contract dipatuhi.

---

## 4) Rencana Struktur File (Disarankan)

Tambahan baru (minimum viable layout):

```text
Source/Visualizer/
  Core/
    VisualFrameTypes.h
    VisualizerConfig.h
    IFrameTransport.h
    AtomicFrameSnapshotTransport.h
  Modules/
    SpectrumModule.h
    SpectrumModule.cpp
```

Catatan:
- Nama final bisa disesuaikan style repo.
- Untuk Stage 1, `AtomicFrameSnapshotTransport` cukup; SPSC ring buffer penuh bisa masuk Stage berikutnya jika dibutuhkan.

---

## 5) Data Contracts (Stage 1 Minimal)

## A. AudioFeatureFrame (producer payload)
Berisi data mentah/near-raw dari audio thread yang cukup untuk visual:
- `frameIndex`
- `numBins` atau fixed-bin buffer index
- magnitudo spectrum (minimal yang dibutuhkan)
- optional: metrik ringkas (peak/rms) bila membantu overlay

## B. VisualFrameSnapshot (consumer payload)
Berisi data siap render:
- versi/frame index terakhir valid
- bins yang sudah dinormalisasi/clamp sesuai range visual
- status validity (`valid/stale`)

Rule penting:
- Snapshot yang dibaca UI dianggap **immutable** untuk siklus render tersebut.

---

## 6) Thread Contract & Realtime Rules

Wajib dijaga selama implementasi:

1. Tidak ada `lock`, heap allocation dinamis, disk I/O, network call di `processBlock()`.
2. Audio thread hanya `publish` data analyzer.
3. UI thread hanya `consume` snapshot analyzer.
4. UI tidak boleh membuat audio thread menunggu.

Transport pattern yang direkomendasikan Stage 1:
- double-buffer/snapshot dengan atomic version flag.
- `publish(frame)` dipanggil dari audio thread.
- `tryConsumeLatest(snapshot)` dipanggil dari UI timer.

---

## 7) Integrasi ke `PluginProcessor` (Tanpa Refactor Besar)

Langkah implementasi yang disarankan:

1. Tambah member transport analyzer di `BULLsEYEProcessor`.
2. Di `prepareToPlay()`: reset transport state.
3. Di `processBlock()`:
   - tetap proses audio existing,
   - hitung/siapkan frame analyzer minimal,
   - publish frame ke transport (non-blocking).
4. Expose API read-only untuk editor, contoh:
   - `bool tryGetLatestVisualSnapshot(VisualFrameSnapshot& out) const`

Catatan:
- Hindari expose pointer mutable shared buffer ke editor.

---

## 8) Integrasi ke `PluginEditor` (Timer Pull)

Langkah implementasi yang disarankan:

1. Tambahkan `SpectrumModule` sebagai member editor.
2. Pada `timerCallback()`:
   - pull snapshot terbaru dari processor,
   - kirim ke `SpectrumModule::consumeSnapshot(...)`,
   - trigger repaint terkontrol.
3. Pada `resized()`:
   - alokasikan area untuk spectrum tanpa merusak layout loudness panel existing.

Cadence awal:
- gunakan default existing **30 Hz** untuk Stage 1.
- 60 Hz hanya opsional jika bukti perf aman.

---

## 9) Render Rules untuk `SpectrumModule`

Minimum behavior Stage 1:
- log-frequency axis (readable)
- dB floor/ceiling clamp
- smoothing ringan agar kurva tidak terlalu jitter
- fallback deterministic jika data stale/tidak tersedia

Fallback yang diterima:
- hold-last-frame dengan decay ringan
- atau freeze ke state netral konsisten

Yang dihindari di Stage 1:
- efek visual berat (multi-pass blur/glow)
- repaint full-window terus-menerus tanpa kontrol

---

## 10) Urutan Implementasi yang Direkomendasikan (Small Safe Commits)

1. **Commit A — Contracts + transport scaffold**
   - tambah `VisualFrameTypes` + transport class
   - belum render spectrum

2. **Commit B — Processor publish path**
   - processor publish analyzer frame ke transport
   - belum dipakai UI

3. **Commit C — Spectrum module basic render**
   - komponen spectrum bisa render dari snapshot statis/test

4. **Commit D — Editor integration**
   - timer pull snapshot nyata dari processor
   - integrasi layout

5. **Commit E — Guardrails & tuning**
   - fallback stale handling
   - smoothing ringan
   - repaint/perf tuning awal

---

## 11) Acceptance Checklist (Stage 1)

- [ ] Build bersih tanpa regression.
- [ ] Playback stabil (tanpa glitch/dropout karena visualizer).
- [ ] Audio thread tetap mematuhi red lines realtime.
- [ ] Spectrum tampil konsisten saat playback aktif.
- [ ] Loudness panel existing tetap akurat dan stabil.
- [ ] Editor open/close berulang aman.
- [ ] UI cadence stabil dalam budget.

---

## 12) Risk Notes & Mitigation

## R1 — Shared mutable analyzer state
Mitigasi: pakai immutable snapshot consumption, bukan pointer writable sharing.

## R2 — Overdraw/repaint cost
Mitigasi: batasi repaint area modul, pertahankan cadence konservatif, profiling bertahap.

## R3 — Scope creep Stage 1
Mitigasi: kunci pada spectrum core + loudness core, tunda spectrogram/phase/GPU.

---

## 13) Referensi Wajib

- `visualizer/INTRODUCTION.md`
- `visualizer/ARCHITECTURE.md`
- `visualizer/STAGE_1_PLAN.md`
- `visualizer/REFERENCE_ADOPTION_STRATEGY.md`
- `visualizer/LAW_EXTRACTION_HAPPYTOWN_SPECTRUM.md`
- `docs/AGENT_MENTAL_MODEL.md`
- `docs/AGENT_OPERATIONAL_CHECKLIST.md`

Dokumen ini harus dibaca sebagai panduan implementasi praktis, bukan pengganti contract utama.

---

## 14) Compliance terhadap AGENT_MENTAL_MODEL

Status: **sebagian besar sudah comply**, dengan penguatan berikut agar eksplisit dan audit-friendly.

### A. Contract-in-Contract Mapping (wajib eksplisit)

1. **Product Contract**
   - Stage 1 hanya menambah spectrum core dan menjaga loudness panel existing tetap stabil.
   - Tidak ada perubahan behavior tersembunyi di metering yang sudah berjalan.

2. **Framework Contract (JUCE)**
   - Integrasi tetap mengikuti boundary lifecycle (`prepareToPlay`, `processBlock`, editor timer).
   - Ownership thread dijaga: audio publish-only, UI consume-only.

3. **API Method Contract**
   - `processBlock()` tetap realtime-safe, deterministic, non-blocking.
   - API editor ke processor bersifat read-only snapshot.

4. **Host + Platform Contract**
   - Perilaku fallback harus konsisten saat snapshot stale/missing lintas host.
   - Open/close editor berulang tidak boleh merusak state.

### B. Priority Rule saat konflik desain

Jika ada trade-off implementasi, urutan keputusan wajib:

**Audio safety > Correctness > UX convenience > Build ergonomics**

### C. Evidence Gate (sebelum klaim done)

Sebelum item Stage 1 dinyatakan selesai, minimal harus ada evidence:
- test/build output relevan,
- validasi tidak ada pelanggaran red lines audio thread,
- uji lifecycle host (open/close editor, playback transition),
- catatan bahwa item non-goal tetap tidak dipromosikan diam-diam.

Catatan:
- Guide ini adalah rencana implementasi. Status "done" hanya sah jika evidence di atas tersedia.

---

## 15) Strict Review Findings (Pengetatan)

Berikut hasil review lebih ketat terhadap guide ini:

### Sudah kuat
- Kontrak realtime sudah dijadikan red line eksplisit.
- Pemisahan ownership audio/UI sudah konsisten.
- Scope Stage 1 dan non-goals sudah relatif terkunci.

### Perlu ditegaskan (ditambahkan pada implementasi nanti)
1. **Preallocation policy**
   - Semua buffer analyzer (bins, smoothing history, path cache) wajib dialokasikan sebelum jalur realtime aktif.
   - Dilarang resize dinamis di jalur audio.

2. **Fallback timeout rule**
   - Definisikan ambang stale snapshot secara eksplisit (mis. berbasis frame version atau elapsed tick UI).
   - Fallback harus deterministic lintas host.

3. **Profiling gate minimum**
   - Catat baseline dan setelah perubahan untuk: CPU UI thread, frame pacing, indikasi glitch/dropout.
   - Jika metrik memburuk signifikan, rollback ke cadence konservatif.

4. **No hidden scope promotion**
   - Spectrogram/phase/GPU tetap non-goal Stage 1 kecuali ada keputusan scope baru tertulis.

---

## 16) Open Decisions (Wajib Diputuskan Sebelum Coding)

Sebelum implementasi kode dimulai, keputusan berikut harus dikunci:

1. **Transport final Stage 1**
   - versioned double-buffer snapshot **atau** SPSC ring buffer tipis.

2. **Bin count & update cadence**
   - jumlah bin visual final + frekuensi update analyzer terhadap block size/sample rate.

3. **Smoothing policy**
   - parameter smoothing default (attack/release atau single-factor) dan batas aman CPU.

4. **Stale behavior final**
   - pilih freeze, hold-last + decay, atau reset-netral (satu policy konsisten).

5. **Layout contract editor**
   - area spectrum minimum agar komponen loudness existing tidak terdegradasi usability-nya.

---

## 17) PR Evidence Bundle (Minimal untuk Review Ketat)

Setiap PR implementasi Stage 1 sebaiknya menyertakan paket evidence minimum berikut:

- Ringkasan perubahan per contract layer (Product/JUCE/API/Host).
- Bukti red line audio aman (statement + titik kode yang terdampak).
- Build result + test result yang relevan.
- Uji lifecycle ringkas: open/close editor berulang + playback transition.
- Catatan metrik sebelum/sesudah untuk cadence dan UI cost.
- Pernyataan eksplisit bahwa non-goals tetap tidak diaktifkan.
