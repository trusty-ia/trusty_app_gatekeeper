/*
 * Copyright 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef TRUSTY_GATEKEEPER_H_
#define TRUSTY_GATEKEEPER_H_

#include <trusty_std.h>
#include <stdio.h>

#include <gatekeeper/gatekeeper.h>

#define LOG_TAG "trusty_gatekeeper"
#define TLOGE(fmt, ...) \
    fprintf(stderr, "%s: %d: " fmt, LOG_TAG, __LINE__,  ## __VA_ARGS__)

#if LOCAL_TRACE
#define TLOGI(fmt, ...) \
    fprintf(stderr, "%s: %d: " fmt, LOG_TAG, __LINE__,  ## __VA_ARGS__)
#else
#define TLOGI(fmt, ...)
#endif

namespace gatekeeper {

class TrustyGateKeeper : public GateKeeper {
public:
    TrustyGateKeeper();

    long OpenSession();
    void CloseSession();
protected:
    // See gatekeeper/gatekeeper.h for documentation

    bool GetAuthTokenKey(const uint8_t **auth_token_key, uint32_t *length) const override;

    void GetPasswordKey(const uint8_t **password_key, uint32_t *length) override;

    void ComputePasswordSignature(uint8_t *signature, uint32_t signature_length,
            const uint8_t *key, uint32_t key_length, const uint8_t *password,
            uint32_t password_length, salt_t salt) const override;

    void GetRandom(void *random, uint32_t requested_size) const override;

    void ComputeSignature(uint8_t *signature, uint32_t signature_length,
            const uint8_t *key, uint32_t key_length, const uint8_t *message,
            const uint32_t length) const override;

    uint64_t GetMillisecondsSinceBoot() const override;

    bool GetFailureRecord(uint32_t uid, secure_id_t user_id, failure_record_t *record,
            bool secure) override;

    bool WriteFailureRecord(uint32_t uid, failure_record_t *record, bool secure) override;

    bool ClearFailureRecord(uint32_t uid, secure_id_t user_id, bool secure) override;

    bool IsHardwareBacked() const override;

private:
    bool SeedRngIfNeeded();
    bool ShouldReseedRng();
    bool ReseedRng();

    long DeriveMasterKey();
    void ClearMasterKey();

    void InitMemoryRecords();
    bool GetMemoryRecord(secure_id_t user_id, failure_record_t *record);
    bool WriteMemoryRecord(failure_record_t *record);
    bool GetSecureFailureRecord(uint32_t uid, secure_id_t user_id,
            failure_record_t *record);
    bool WriteSecureFailureRecord(uint32_t uid, failure_record_t *record);

    UniquePtr<uint8_t[]> master_key_;
    bool rng_initialized_;
    int calls_since_reseed_;

    int num_mem_records_;
    UniquePtr<failure_record_t[]> mem_records_;
};

}

#endif // TRUSTY_GATEKEEPER_H_
