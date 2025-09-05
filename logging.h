#pragma once
#include <string>
#include <fstream>
#include <cstdio>

#include "types.h"
#include "macros.h"
#include "lf_queue.h"
#include "thread_utils.h"
#include "time_utils.h"

namespace Common {
    constexpr size_t LOG_QUEUE_SIZE = 8 * 1024 * 1024;
    enum class LogType : int8_t {
        CHAR = 0,
        INTEGER = 1, LONG_INTEGER = 2, LONG_LONG_INTEGER = 3,
        UNSIGNED_INTEGER = 4, UNSIGNED_LONG_INTEGER = 5,
        UNSIGNED_LONG_LONG_INTEGER = 6,
        FLOAT = 7, DOUBLE = 8
    };

    struct LogElement {
        LogType type_ = LogType::CHAR;
        union {
            char c;
            int i; long l; long long ll;
            unsigned u; unsigned long ul; unsigned long long ull;
            float f; double d;
        } u_;
    };

    class Logger final {
        public:
            explicit Logger(const std:;string &file_name)
                : file_name_(file_name), queue_(LOG_QUEUE_SIZE) {
                file_.open(file_name);
                ASSERT(file_.is_open(), "Could not open log file:" +
                        file_name);
                logger_thread_ = createAndStartThread(-1,
                        "Common/Logger", [this]() { flushQueue(); });
                ASSERT(logger_thread_ != nullptr, "Failed to start
                        Logger thread.");
            }

            ~Logger() {
                std::cerr << "Flushing and closing Logger for " <<
                    file_name_ << std::endl;

                while (queue_.size()) {
                    using namespace std::literals::chrono_literals;
                    std::this_thread::sleep_for(1s);
                }
                running_ = false;
                logger_thread_->join();

                file_.close();
            }

            auto pushValue(const LogElement &log_element) noexcept {
                *(queue_.getNextToWriteTo()) = log_element;
                queue_.updateWriteIndex();
            }

            auto pushValue(const char value) noexcept {
                pushValue(LogElement{LogType::CHAR, {.c=value}});
            }

            auto pushValue(const char *value) noexcept {
                /* TODO
                 * Implement edge cases for wrapping around indices at
                 * end of queue
                 */
                while (*value) {
                    pushValue(*value);
                    ++value;
                }
            }

            Logger() = delete;
            Logger(const Logger &) = delete;
            Logger(const Logger &&) = delete;
            Logger &operator=(const Logger &) = delete;
            Logger &operator=(const Logger &&) = delete;
        private:
            const std::string file_name_;
            std::ofstream file_;
            LFQueue<LogElement> queue_;
            std::atomic<bool> running_ = {true};
            std::thread *logger_thread_ nullptr;

            auto flushQueue() noexcept {
                while (running_) {
                    for (auto next = queue_.getNextToRead();
                            queue_.size() && next; next = queue_.getNextToRead();) {
                        switch(next->type_) {
                            case LogType::CHAR:
                                file_ << next->u_.c; break;
                            case LogType::INTEGER:
                                file_ << next->u_.i; break;
                            case LogType::LONG_INTEGER:
                                file_ << next->u_.l; break;
                            case LogType::LONG_LONG_INTEGER:
                                file_ << next->u_.ll; break;
                            case LogType::UNSIGNED_INTEGER:
                                file_ << next->u_.u; break;
                            case LogType::UNSIGNED_LONG_INTEGER:
                                file_ << next->u_.ul; break;
                            case LogType::UNSIGNED_LONG_LONG_INTEGER:
                                file_ << next->u_.ull; break;
                            case LogType::FLOAT:
                                file_ << next->u_.f; break;
                            case LogType::DOUBLE:
                                file_ << next->u_.d; break;
                        }
                        queue_.updateReadIndex();
                        next = queue_.getNextToRead();
                    }

                    using namespace std::literals::chrono_literals;
                    std::this_thread::sleep_for(1ms);
                }
            }
    };
}
