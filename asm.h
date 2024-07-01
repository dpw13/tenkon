static inline void __attribute__((noreturn)) exc_ret(void) {
    asm("rte");
    __builtin_unreachable();
}

static inline void __attribute__((noreturn)) cpu_stop(void) {
    asm("stop #0x2700");
    __builtin_unreachable();
}

/* Disable tracing */
static inline void trace_none(void) {
    asm("andi #0x3FFF,%sr");
}

/* Set T0 = 1 (trace on change of flow) */
static inline void trace_flow(void) {
    asm("andi #0x7FFF,%sr");
    asm("ori #0x4000,%sr");
}
                
/* Set T1 = 1 (trace all instructions) */
static inline void trace_all(void) {
    asm("andi #0xBFFF,%sr");
    asm("ori #0x8000,%sr");
}
