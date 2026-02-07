# REFERENCE ADOPTION STRATEGY

Dokumen ini mendefinisikan cara mengadopsi praktik terbaik dari codebase visualizer eksternal untuk BULLsEYE, tanpa menyalin risiko tersembunyi.

---

## 1) Prinsip Utama

**Adopt laws/patterns, not baggage.**

Yang diadopsi:
- arsitektur thread-safe,
- pola data flow,
- strategi rendering/perf,
- contract discipline.

Yang dihindari:
- copy-paste mentah modul besar,
- coupling ke dependency yang tidak perlu,
- asumsi lifecycle host yang tidak kompatibel,
- lisensi yang tidak jelas.

---

## 2) Tujuan Strategi

1. Mempercepat pengembangan visualizer dengan referensi proven.
2. Menjaga kontrak BULLsEYE tetap aman (realtime, lifecycle, determinism).
3. Menghasilkan implementasi final yang maintainable dan legal-compliant.

---

## 3) Kriteria Seleksi Reference Codebase

Gunakan kandidat yang memenuhi sebagian besar kriteria berikut:

- **A. Realtime Safety Evidence**
  - Tidak melakukan blocking work di audio callback.
  - Punya transport lock-free atau snapshot atomic.

- **B. Thread Model Clarity**
  - Ownership data antar thread jelas.
  - UI rendering tidak mengganggu audio thread.

- **C. Visual Quality + Performance**
  - Analyzer stabil pada beban playback nyata.
  - Ada strategi frame pacing/repaint control.

- **D. Maintenance Quality**
  - Struktur modul rapi.
  - Readability dan testability memadai.

- **E. License Compatibility**
  - Lisensi source kompatibel untuk adopsi.
  - Tidak ada ambiguitas legal.

---

## 4) Adoption Workflow

### Step 1 — Candidate Discovery
- Kumpulkan 2–5 referensi visualizer yang matang.
- Prioritaskan yang serupa stack (C++/JUCE/audio plugin).

### Step 2 — Law Extraction
- Ekstrak “laws” dari tiap kandidat:
  - thread law,
  - data transport law,
  - render law,
  - lifecycle law.

### Step 3 — Gap Mapping (Against BULLsEYE Contracts)
- Cocokkan ke:
  - `docs/AGENT_MENTAL_MODEL.md`
  - `docs/AGENT_OPERATIONAL_CHECKLIST.md`
  - `visualizer/ARCHITECTURE.md`

### Step 4 — Selective Reimplementation
- Re-implement core pattern yang lolos evaluasi.
- Hindari transplant besar tanpa simplifikasi.

### Step 5 — Evidence Gate
- Validasi hasil adopsi dengan metrik dan checklist kontrak.

---

## 5) Contract Gate (Wajib Lolos)

Sebelum pattern dianggap “adopted”, harus lolos:

1. **Realtime Contract Gate**
   - No lock/alloc/I-O/network di `processBlock()`.

2. **Thread Contract Gate**
   - Audio publish-only, UI consume-only.
   - Snapshot immutable per block/frame.

3. **Lifecycle Gate**
   - Aman pada open/close editor, transport restart, DAW reload.

4. **Deterministic Fallback Gate**
   - Data stale/missing tidak menimbulkan perilaku acak.

5. **Performance Gate**
   - UI CPU budget dan frame pacing dalam batas target.

6. **Legal Gate**
   - Lisensi referensi terverifikasi kompatibel.

---

## 6) Risk Register (Adoption Risks)

- **R1: Hidden thread assumption**
  - Mitigasi: thread map + code review kontrak sebelum merge.

- **R2: Perf drop pada host tertentu**
  - Mitigasi: test matrix lintas DAW + platform.

- **R3: License conflict**
  - Mitigasi: legal review sebelum import struktur/signifikan code.

- **R4: Overengineering**
  - Mitigasi: fase bertahap (Stage 1 core dulu).

---

## 7) Adoption Artifact Checklist

- [ ] Daftar kandidat referensi + alasan pemilihan.
- [ ] Matriks law extraction per kandidat.
- [ ] Gap mapping terhadap kontrak BULLsEYE.
- [ ] Rencana reimplementation per modul.
- [ ] Hasil validasi kontrak + metrik performa.

---

## 8) Keputusan Eksekusi

Default policy:
- **Prefer reimplementation berbasis law**.
- **Direct code adoption hanya jika:**
  - legal jelas,
  - kontrak teknis kompatibel,
  - dan biaya adaptasi < biaya rewrite.
