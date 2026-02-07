# AGENT OPERATIONAL CHECKLIST (1 HALAMAN)

**Tujuan:** Checklist cepat sebelum, saat, dan setelah perubahan pada BULLsEYE (khususnya alur Sprint 5 licensing).

---

## A. PRE-FLIGHT (Sebelum Ubah Kode)

- [ ] Identifikasi fitur/issue yang dikerjakan (scope jelas, tidak melebar).
- [ ] Petakan 4 lapisan kontrak terdampak:
  - [ ] Product Contract
  - [ ] JUCE Framework Contract
  - [ ] API Method Contract
  - [ ] Host/Platform Contract
- [ ] Definisikan pass/fail criteria per kontrak (apa bukti berhasil/gagal).
- [ ] Tandai item **TBD scope** secara eksplisit (jangan dianggap selesai implisit).

---

## B. RED LINES WAJIB (HARUS SELALU LOLOS)

### Audio Thread / `processBlock()`
- [ ] Tidak ada lock/mutex.
- [ ] Tidak ada heap allocation.
- [ ] Tidak ada disk I/O.
- [ ] Tidak ada network call.
- [ ] Hanya read atomic/lock-free untuk cek lisensi.
- [ ] Perilaku tetap deterministik.

### License SSOT
- [ ] Sumber kebenaran tunggal: snapshot lisensi atomik di EngineCore.
- [ ] Update state hanya lewat commit/swap API EngineCore.
- [ ] Publish hanya snapshot lengkap (tidak ada partial publish).
- [ ] Snapshot lisensi **immutable per audio block** (tidak ada mutasi di tengah block).

### Fallback Deterministik
- [ ] Jika state lisensi unknown/stale/unavailable → fallback **TRIAL**.
- [ ] Fallback identik lintas host/platform.

---

## C. IMPLEMENTASI (Saat Ubah Kode)

- [ ] Perubahan minimal, terarah, dan contract-driven.
- [ ] Tidak ada perubahan behavior tersembunyi.
- [ ] Inisialisasi engine di `prepareToPlay()` (bukan first `processBlock()`).
- [ ] UI/background task (aktivasi/refresh/network) tetap off audio thread.
- [ ] UI refresh bounded (target ±1 Hz untuk status yang tidak realtime-kritis).
- [ ] `getStateInformation()` / `setStateInformation()` konsisten terhadap state lisensi.
- [ ] Serialization state **tidak memicu validasi ulang / network / refresh**.

---

## D. VERIFIKASI SPRINT 5 (C1–C6)

- [ ] **C1 Audio:** lisensi check non-blocking + atomic di audio path.
- [ ] **C2 UI:** operasi status/aktivasi di thread aman.
- [ ] **C3 Lifecycle Host:** boundary init/restore teardown jelas dan aman.
- [ ] **C4 Persistence/Security:** survive reload, corrupt data → graceful trial fallback.
- [ ] **C5 Packaging:** macOS bundle/rpath/codesign valid; Windows/Linux sesuai scope aktif.
- [ ] **C5 Packaging Evidence:** tidak ada absolute path (`otool -L`, `dumpbin`, `ldd` bersih).
- [ ] **C6 Testing:** regression hijau + tes lisensi baru lengkap.

Tes minimum wajib:
- [ ] Perubahan state lisensi saat playback aktif.
- [ ] Reload DAW dengan blob lisensi corrupt.

---

## E. POST-FLIGHT (Sebelum Merge)

- [ ] Jalankan contradiction check (status, phase naming, scope labels).
- [ ] Revalidasi red lines audio thread.
- [ ] Dokumentasikan evidence:
  - [ ] Test output
  - [ ] Command/build logs relevan
  - [ ] Artifact/package checks relevan
- [ ] Pastikan item TBD tetap ditandai TBD (tidak dipromosikan tanpa keputusan resmi).

---

## F. RULE OF PRIORITY (Saat Konflik)

- [ ] Terapkan urutan keputusan: **Audio safety > Correctness > UX convenience > Build ergonomics**.

---

## G. ESCALATION GATE

- [ ] Jika ada **keraguan sekecil apa pun** soal audio-thread contract: **STOP, revert, review**.

---

## H. ENFORCEMENT RULE

- [ ] Checklist dilanggar = **PR tidak direview**.
