#include "button_filter.h"
uint8_t button_filter_step(ButtonFilter *b,uint8_t pressed,uint8_t repeat,uint32_t now) {
    /* Moi lan mau doi, dem lai 30 ms. Nhan va nha deu phai on dinh. */
    if (pressed != b->sample) { b->sample=pressed; b->changed_at=now; }
    if (pressed != b->stable && (uint32_t)(now-b->changed_at)>=30U) {
        b->stable=pressed; b->repeat_at=now+300U;
        if (pressed) return 1U;
    }
    /* Stop repeat as soon as release is sampled, even during release debounce. */
    /* Tra 1 = press, 2 = repeat, 0 = khong co su kien.
     * So sanh deadline bang hieu co dau de xu ly tick uint32_t quay vong. */
    if (repeat && pressed && b->stable && (int32_t)(now-b->repeat_at)>=0) {
        b->repeat_at=now+100U; return 2U;
    }
    return 0U;
}
