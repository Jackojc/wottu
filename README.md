# `wottu`
> A term rewriting language aimed at text processing

wottu is an experimental offshoot of wot++ that uses term rewriting instead
of function application.

wottu draws heavy inspiration from [mlatu](https://github.com/mlatu-lang/mlatu).

Like its predecessor, wottu reduces from a higher level
representation of a document down to a single large string of
text. In wottu, all terms must be reduced down to a series
of strings which are then concatenated to produce the final
document. Any terms which are not rewritten will result in
an error.

# Build
```sh
$ git clone https://github.com/Jackojc/wottu.git --recursive && cd wottu/
$ pre-commit install  # Checks for development
$ meson setup build/
$ meson compile -C build/
$ meson test -C build/
```

> Note: you can use a different compiler/linker with meson when running setup
by using the environment variables `CC=foo` and `CC_LD=foo`.

> Note: enable UBSAN & ASAN using `meson configure -Dsanitizers=true build/`
