# VISUALIZER INTRODUCTION

## Tujuan

Direktori `visualizer/` disiapkan sebagai pondasi pengembangan visualizer BULLsEYE yang modern, modular, dan tetap aman terhadap kontrak audio realtime.

Targetnya bukan sekadar “UI bagus”, tapi **visual feedback yang informatif** tanpa melanggar red lines audio thread.

---

## Prinsip Inti

1. **Realtime-Safe First**
   - Audio thread hanya menulis data metering/analyzer secara lock-free.
   - Tidak ada lock, alloc, disk I/O, atau network call di jalur audio.

2. **Snapshot-Based UI Consumption**
   - UI membaca snapshot data yang sudah dipublikasikan aman.
   - Hindari coupling langsung UI ↔ proses audio per-sample.

3. **Deterministic Behavior**
   - Saat data analyzer tidak tersedia/stale, UI fallback ke state aman yang konsisten.

4. **Performance Budget Awareness**
   - Refresh UI terukur (mis. 30/60 FPS adaptif).
   - Optimasi repaint, caching path/mesh, dan downsampling analyzer bila perlu.

---

## Ownership Model (Kapan pakai `std::unique_ptr`)

Gunakan **`std::unique_ptr`** jika komponen visualizer:
- opsional (enable/disable per mode),
- lazy-initialized,
- dapat diganti runtime,
- atau butuh lifecycle dinamis.

Gunakan **by-value member** jika komponen selalu ada sepanjang umur editor.

> `unique_ptr` direkomendasikan untuk modularitas, tapi bukan syarat tunggal kualitas visualizer.

---

## Roadmap Bertahap

### V1 — Stabil Dasar
- Meter/analyzer dasar realtime-safe.
- Data path lock-free + timer UI stabil.
- Validasi beban CPU UI dan kelancaran repaint.

### V2 — Modular
- Komponen analyzer dipisah per fitur (spectrum, spectrogram, phase scope).
- Enable/disable komponen via mode/setting.
- `std::unique_ptr` dipakai untuk modul dinamis.

### V3 — Advanced UX
- Smoothing canggih, psychoacoustic mapping, visual polish tingkat lanjut.
- Evaluasi backend rendering akselerasi (opsional) dengan fallback CPU aman.

---

## Acceptance Criteria Awal

- Build bersih tanpa regresi.
- Audio thread tetap mematuhi red lines.
- UI visualizer responsif dan tidak mengganggu playback.
- Performa terukur: first-frame, repaint cost, CPU UI thread.
- Dokumentasi keputusan arsitektur tercatat sebelum ekspansi fitur.

---

## Catatan

Folder ini adalah ruang kerja evolusi visualizer. Source of truth tetap:
- kode aktif di `Source/`
- kontrak kerja di `docs/AGENT_MENTAL_MODEL.md`
- checklist operasional di `docs/AGENT_OPERATIONAL_CHECKLIST.md`
