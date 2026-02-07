# VISUALIZER DOCS HUB

Folder `visualizer/` adalah pusat dokumentasi pengembangan visualizer BULLsEYE.

## Isi Dokumen

- `INTRODUCTION.md`
  - Gambaran umum tujuan visualizer, prinsip realtime-safe, dan roadmap bertahap.

- `ARCHITECTURE.md`
  - Blueprint arsitektur modular (L0–L5), contract matrix, ownership rules, dan cost classification.

- `STAGE_1_PLAN.md`
  - Scope eksekusi Tahap 1 (Spectrum + Loudness core), non-goals, acceptance criteria, dan exit gate.

- `REFERENCE_ADOPTION_STRATEGY.md`
  - Strategi adopsi referensi eksternal dengan prinsip **adopt laws/patterns, not baggage**.

- `LAW_EXTRACTION_HAPPYTOWN_SPECTRUM.md`
  - Hasil law extraction dari kandidat repo `happytown-s/Spectrum-Analyzer-Plugin`.

---

## Urutan Baca yang Disarankan

1. `INTRODUCTION.md`
2. `ARCHITECTURE.md`
3. `STAGE_1_PLAN.md`
4. `REFERENCE_ADOPTION_STRATEGY.md`
5. `LAW_EXTRACTION_HAPPYTOWN_SPECTRUM.md`

---

## Prinsip Eksekusi

- Audio thread safety non-negotiable (no lock/alloc/I-O/network di `processBlock()`).
- Data analyzer harus dipindah lintas thread secara aman (snapshot/lock-free).
- Stage 1 fokus komponen biaya rendah-menengah; item biaya tinggi perlu perf evidence.

---

## Source of Truth Terkait

- `docs/AGENT_MENTAL_MODEL.md`
- `docs/AGENT_OPERATIONAL_CHECKLIST.md`
- Kode aktif: `Source/`
