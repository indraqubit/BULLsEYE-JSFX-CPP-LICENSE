# VISUALIZER ARCHITECTURE (BLUEPRINT)

Dokumen ini adalah blueprint arsitektur modular visualizer untuk BULLsEYE.
Fokus: **realtime-safe**, modular, dan terukur performanya.

---

## 1) End-to-End Flow

```text
Audio Thread (processBlock)
    └─> L0 Feature Extractor (RT-safe)
          └─> L1 Lock-Free Transport (ring buffer / atomic snapshot)
                └─> L2 Visual Domain Transform (normalize/map/smooth prep)
                      └─> L4 Orchestrator (module routing + lifecycle)
                            └─> L3 Render Modules (Spectrum/Spectrogram/Phase/Meter)
                                  └─> L5 PluginEditor Integration (timer/repaint/layout)
```

Prinsip aliran:
- Audio thread **publish only**.
- UI thread **consume + render only**.
- Tidak ada shared mutable state lintas thread tanpa mekanisme lock-free/atomic yang tepat.

---

## 2) Layer Responsibilities

## L0 — Audio Feature Extractor (RT-Critical)
- Input: block audio.
- Output: feature frame mentah (RMS/peak/FFT bins dsb).
- Aturan keras:
  - no lock,
  - no heap allocation,
  - no disk/network I/O,
  - deterministic per block.

## L1 — Transport Layer
- Menjembatani audio thread → UI thread.
- Mekanisme: SPSC ring buffer atau atomic snapshot swap.
- Audio thread tidak boleh pernah menunggu reader.

## L2 — Visual Domain Layer
- Mapping dan transform visual:
  - linear→log frequency,
  - amplitude→dB scale,
  - floor/ceiling clamp,
  - optional smoothing prep.
- Pure transform, tanpa side-effect ke audio engine.

## L3 — Render Modules
- Modul independen:
  - `SpectrumModule`
  - `SpectrogramModule`
  - `PhaseScopeModule`
  - `LoudnessMeterModule`
- Modul merender dari snapshot immutable.

## L4 — Orchestrator Layer
- Registry module, mode switching, lifecycle management.
- Mengatur modul aktif/nonaktif.
- Tempat utama pemakaian `std::unique_ptr` untuk modul dinamis.

## L5 — Integration Layer (PluginEditor)
- Attach/detach visualizer ke editor.
- Timer cadence (30/60 Hz adaptif), resize, repaint policy.
- Tidak mengubah kontrak audio processing.

---

## 3) Contract Matrix (Visualizer)

### C1 — Realtime Contract
- `processBlock()` tetap realtime-safe.
- Snapshot analyzer immutable per audio block.

### C2 — Thread Ownership Contract
- Audio thread: write-only ke transport.
- UI thread: read-only snapshot untuk render.

### C3 — Lifecycle Contract
- Init/reset sinkron dengan lifecycle host (`prepareToPlay`, transport transitions).
- Editor open/close tidak menyebabkan state corruption.

### C4 — Deterministic Fallback Contract
- Jika snapshot kosong/stale: fallback visual konsisten (freeze/decay/default state).

### C5 — Performance Contract
- UI CPU budget terukur.
- Repaint terkendali (dirty-region/cached path jika perlu).

### C6 — Modularity Contract
- Enable/disable modul tanpa menyentuh core audio.
- Dynamic module ownership via `std::unique_ptr`.

---

## 4) Ownership Rules

Gunakan **by-value** bila komponen:
- selalu ada selama umur editor,
- ringan dan tidak perlu swap runtime.

Gunakan **`std::unique_ptr`** bila komponen:
- optional/feature-flag,
- lazy-initialized,
- polymorphic,
- sering di-swap berdasarkan mode.

---

## 5) Suggested Folder Layout

```text
visualizer/
  INTRODUCTION.md
  ARCHITECTURE.md
  core/
    VisualFrameTypes.h
    VisualizerConfig.h
    IFrameTransport.h
  extractors/
    IFeatureExtractor.h
    RMSPeakExtractor.h
    FFTExtractor.h
  modules/
    IVisualizerModule.h
    SpectrumModule.h
    SpectrogramModule.h
    PhaseScopeModule.h
    LoudnessMeterModule.h
  orchestrator/
    VisualizerOrchestrator.h
  render/
    VisualizerPainter.h
    CachedPathStore.h
```

---

## 6) Interface Skeleton (Draft)

```cpp
// core/VisualFrameTypes.h
struct AudioFeatureFrame {
    uint64_t frameIndex;
    float rmsL, rmsR;
    float peakL, peakR;
    // optional: FFT bins pointer/index
};

struct VisualFrameSnapshot {
    uint64_t frameIndex;
    // normalized/log-mapped values ready for rendering
};

// core/IFrameTransport.h
class IFrameTransport {
public:
    virtual ~IFrameTransport() = default;
    virtual bool push(const AudioFeatureFrame& frame) = 0; // audio thread
    virtual bool pop(AudioFeatureFrame& out) = 0;          // UI thread
};

// modules/IVisualizerModule.h
class IVisualizerModule {
public:
    virtual ~IVisualizerModule() = default;
    virtual void prepare(double sampleRate, int maxBlockSize) = 0;
    virtual void consumeSnapshot(const VisualFrameSnapshot& snapshot) = 0;
    virtual void paint(juce::Graphics& g, juce::Rectangle<int> bounds) = 0;
    virtual void resized(juce::Rectangle<int> bounds) = 0;
};
```

Catatan:
- Interface ini draft arsitektural, bukan kontrak final API.
- Final naming harus menyesuaikan SSOT dan style codebase.

---

## 7) Validation Checklist (Minimum)

- [ ] Tidak ada regression audio glitch/dropout saat visualizer aktif.
- [ ] UI frame pacing stabil pada target FPS.
- [ ] Tidak ada blocking di audio thread.
- [ ] Mode switch modul berjalan tanpa crash/leak.
- [ ] Editor open/close berulang tetap stabil.

---

## 8) Cost Classification (Low / Medium / High)

Klasifikasi ini dipakai untuk keputusan prioritas implementasi dan budget performa.

### Low Cost
- Atomic readiness flag / version counter.
- Snapshot handoff sederhana (tanpa copy besar berulang).
- Timer UI 30 Hz.
- Komponen statis by-value (layout ringan, repaint lokal).

### Medium Cost
- Log-frequency remap per frame.
- Peak-hold + smoothing moderat.
- Multi-layer grid + marker overlay.
- Dynamic module switching via `std::unique_ptr` (tanpa heavy redraw).

### High Cost
- FFT besar dengan frekuensi update tinggi.
- Spectrogram history buffer panjang + scrolling terus-menerus.
- Full-window repaint terus-menerus.
- Efek visual berat (multi-pass glow/blur/gradient kompleks).
- 60 FPS konstan tanpa adaptasi budget.

### Rule Operasional
- Stage 1 default: gunakan item **Low + Medium terkontrol**.
- Item **High** wajib lewat perf gate dan evidence (CPU UI, frame pacing, audio safety).
