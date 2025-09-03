# https://mesonbuild.com/Builtin-options.html

project(
  'wottu',
  'c',

  license: 'AGPL-3.0-only',
  version: '1.0',

  default_options: [
    'c_std=c17',
    'buildtype=release',
    'strip=true',
    'warning_level=2',  # 3=pedantic

    'b_ndebug=if-release',
  ]
)

wottu_name = meson.project_name()
wottu_version = meson.project_version()
wottu_description = 'A term rewriting language aimed at text processing'

wottu_hdr = files(
  'include/wottu/wottu.h',
)

wottu_src = files(
  'src/wottu.c',
)

wottu_inc = include_directories('include/')

wottu_deps = []  # Libraries
wottu_opts = []  # C build options

meson_opts = []  # Meson options

# Set error limits.
if meson.get_compiler('c').get_id() == 'clang'
	add_project_arguments('-ferror-limit=2', language: 'c')

	add_project_arguments('-Wno-unused-function', language: 'c')
	add_project_arguments('-Wno-unused-variable', language: 'c')
	add_project_arguments('-Wno-unused-parameter', language: 'c')

elif meson.get_compiler('c').get_id() == 'gcc'
	add_project_arguments('-fmax-errors=2', language: 'c')

	add_project_arguments('-Wno-unused-function', language: 'c')
	add_project_arguments('-Wno-unused-variable', language: 'c')
	add_project_arguments('-Wno-unused-parameter', language: 'c')
endif

# Sanitizers.
if get_option('sanitizers')
	meson_opts += 'b_sanitize=address,undefined'
	wottu_deps += meson.get_compiler('c').find_library('asan', required: true)
endif

# Build library
wottu = executable(
	'wottu',
	wottu_src,

	include_directories: wottu_inc,
	dependencies: wottu_deps,

	override_options: meson_opts,
	c_args: wottu_opts,

	install: true,
)

# Unit tests
if not meson.is_subproject()
	wottu_tests = {
	  'pass': { 'path': 'tests/pass.c', 'environment': [], 'arguments': [], 'should_fail': false },
	  'fail': { 'path': 'tests/fail.c', 'environment': [], 'arguments': [], 'should_fail': true },
	}

	foreach name, fields: wottu_tests
		path = fields['path']
		should_fail = fields['should_fail']
		environment = fields['environment']
		arguments = fields['arguments']

		exe = executable(
			name,
			path,

			include_directories: wottu_inc,
			dependencies: wottu_deps,

			override_options: meson_opts,
			c_args: wottu_opts,

			install: false,
		)

		test(
			name,
			exe,

			depends: wottu,

			should_fail: should_fail,
			verbose: false,

			args: arguments,
			env: environment,
		)
	endforeach
endif
