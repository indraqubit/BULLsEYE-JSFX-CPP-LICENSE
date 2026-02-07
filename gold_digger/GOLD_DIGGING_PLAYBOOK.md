# GOLD DIGGING PLAYBOOK

Dokumen ini mendefinisikan metode reusable untuk melakukan “gold digging” pada codebase:

> cari emasnya (laws/patterns yang proven), jangan angkut tanahnya (baggage/risiko yang tidak perlu).

---

## 1) Outcome yang Diinginkan

Setiap adopsi dari referensi eksternal harus menghasilkan:
- arsitektur yang lebih kuat,
- implementasi yang lebih maintainable,
- tanpa melanggar red lines kontrak internal.

---

## 2) Prinsip Utama

1. **Adopt laws/patterns, not baggage**
2. **Contract-first execution**
3. **Incremental delivery (small safe commits)**
4. **Evidence-based done criteria**

---

## 3) End-to-End Flow

## Step A — Candidate Discovery
- Kumpulkan referensi 2–5 kandidat yang relevan stack/domain.
- Prioritaskan yang punya bukti stabilitas + lisensi jelas.

Output:
- daftar kandidat + alasan pemilihan.

## Step B — Law Extraction
- Ekstrak hukum/pattern dari tiap kandidat:
  - thread law,
  - data transport law,
  - lifecycle law,
  - render/perf law.

Output:
- law extraction matrix (adopt / adapt / reject).

## Step C — Contract Mapping
- Peta tiap law ke kontrak internal:
  - Product contract,
  - Framework contract,
  - API contract,
  - Host/platform contract.

Output:
- contract compatibility map + risk notes.

## Step D — Selective Reimplementation
- Reimplement pattern yang lolos gate.
- Hindari transplant besar/copy mentah.
- Gunakan commit kecil dan reversible.

Output:
- implementation plan + sequence commit.

## Step E — Evidence Gate
- Verifikasi hasil terhadap contract checklist + metrik.
- Jangan klaim done tanpa evidence.

Output:
- test/build/perf/lifecycle evidence bundle.

---

## 4) Mandatory Gates

Sebelum adopsi dianggap valid, harus lolos:

1. **Realtime Gate**
   - no lock/alloc/I-O/network di jalur realtime (`processBlock()` untuk plugin).

2. **Thread Ownership Gate**
   - ownership jelas (producer/consumer), no hidden mutable sharing.

3. **Lifecycle Gate**
   - aman untuk init/reset/open-close/reload flow host.

4. **Deterministic Fallback Gate**
   - data/state stale tidak memicu perilaku acak.

5. **Performance Gate**
   - UI/audio budget terjaga; frame pacing stabil.

6. **Legal Gate**
   - lisensi referensi diverifikasi kompatibel.

---

## 5) Decision Priority Rule

Jika terjadi konflik desain:

**Audio safety > Correctness > UX convenience > Build ergonomics**

---

## 6) Common Anti-Patterns (Harus Dihindari)

- Copy-paste besar tanpa contract mapping.
- Menandai “done” sebelum evidence tersedia.
- Scope creep (fitur high-cost masuk tanpa perf gate).
- Shared mutable state lintas thread tanpa ownership rule.

---

## 7) Minimal Evidence Bundle

Setiap adopsi/reimplementation minimal menyertakan:
- ringkasan perubahan per contract layer,
- build + test result,
- validasi red lines realtime,
- uji lifecycle penting,
- metrik performa sebelum/sesudah,
- status non-goals tetap non-goals.

---

## 8) Done Criteria

Sebuah pekerjaan “gold digging” dinyatakan selesai jika:
- law extraction terdokumentasi,
- implementasi mengikuti contract,
- seluruh mandatory gates lolos,
- evidence bundle lengkap.
