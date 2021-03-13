yaml_dep = dependency('yaml-0.1')

configure_file(input : 'validate.yml',
        output : 'validate.yml',
        copy: true)

tests = [
  'length',
  'entities',
  'twitter_compliance'
]

foreach test_name : tests
  testcase = executable(
    test_name,
    test_name + '.c',
    dependencies: [libtl_dep, yaml_dep],
  )
  test(test_name, testcase)
endforeach
