/*
 * Copyright (C) 2023 The Android Open Source Project
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

#pragma once

#include <sys/types.h>

#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "common/libs/utils/result.h"
#include "host/commands/cvd/types.h"

namespace cuttlefish {

struct RunCvdProcInfo {
  pid_t pid_;
  std::string home_;
  std::string exec_path_;
  cvd_common::Envs envs_;
  cvd_common::Args cmd_args_;
  std::string stop_cvd_path_;
  bool is_cvd_server_started_;
};

class RunCvdProcessManager {
 public:
  static Result<RunCvdProcessManager> Get();
  RunCvdProcessManager(const RunCvdProcessManager&) = delete;
  RunCvdProcessManager(RunCvdProcessManager&&) = default;
  void ShowAll();
  Result<void> KillAllCuttlefishInstances(const bool cvd_server_children_only,
                                          const bool clear_runtime_dirs) {
    auto stop_cvd_result =
        RunStopCvdForEach(cvd_server_children_only, clear_runtime_dirs);
    if (!stop_cvd_result.ok()) {
      LOG(ERROR) << stop_cvd_result.error().Message();
    }
    CF_EXPECT(SendSignals(cvd_server_children_only));
    return {};
  }

 private:
  RunCvdProcessManager() = default;
  static Result<void> RunStopCvd(const RunCvdProcInfo& run_cvd_info,
                                 const bool clear_runtime_dirs);
  Result<void> RunStopCvdForEach(const bool cvd_server_children_only,
                                 const bool clear_runtime_dirs);
  Result<void> SendSignals(const bool cvd_server_children_only);
  Result<std::vector<RunCvdProcInfo>> CollectInfo();
  std::vector<RunCvdProcInfo> run_cvd_processes_;
};

struct DeviceClearOptions {
  bool cvd_server_children_only;
  bool clear_instance_dirs;
};

/*
 * Runs stop_cvd for all cuttlefish instances found based on run_cvd processes,
 * and send SIGKILL to the run_cvd processes.
 *
 * If cvd_server_children_only is set, it kills the run_cvd processes that were
 * started by a cvd server process.
 */
Result<void> KillAllCuttlefishInstances(const DeviceClearOptions& options);

}  // namespace cuttlefish
