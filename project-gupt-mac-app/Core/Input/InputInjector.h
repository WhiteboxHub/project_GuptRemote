#ifndef INPUT_INJECTOR_H
#define INPUT_INJECTOR_H

#include "../../Shared/Protocol.h"
#include <cstdint>

namespace gupt {
namespace core {
namespace input {

class InputInjector {
public:
    InputInjector();
    ~InputInjector();

    bool Initialize();
    void IngestMouseEvent(const gupt::shared::MouseEvent& ev);
    void IngestKeyboardEvent(const gupt::shared::KeyboardEvent& ev);

private:
    float m_ScreenWidth;
    float m_ScreenHeight;
};

} // namespace input
} // namespace core
} // namespace gupt

#endif // INPUT_INJECTOR_H
