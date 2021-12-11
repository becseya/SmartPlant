#pragma once

#include <cstdint>
#include <functional>

namespace Lora {

using receive_handler_t = std::function<void(uint8_t port, const uint8_t* buffer, size_t size)>;
using message_builder_t = std::function<int16_t(uint8_t* buffer, size_t size)>;

int  init();
void infiniteLoop();

void onReceive(receive_handler_t callback);
void setMessageBuilder(message_builder_t callback);

} // namespace Lora
