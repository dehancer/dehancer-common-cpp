Pod::Spec.new do |s|

  s.name         = "DehancerCommon"
  s.version      = "0.2.1"
  s.summary      = "Dehancer Common SDK"
  s.description  = "Dehancer Common SDK"

  s.homepage     = "https://dehancer.com"

  s.license      = { :type => "MIT", :file => "LICENSE" }
  s.authors            = { "denis svinarchuk" => "denn.nevera@gmail.com" }
  s.social_media_url   = "https://dehancer.com"

  s.platform     = :ios
  s.platform     = :osx

  s.ios.deployment_target = "11.0"
  s.osx.deployment_target = "10.12"
 
  s.swift_version = "4.2"

  s.source       = { :git => "https://github.com/dehancer/dehancer-common-cp", :tag => "#{s.version}" }

  s.source_files  = "platforms/ios/dehancer-common/Classes/*.{h,m,mm}",
                    "src/*.cpp",
                    "include/**/*.h",
                    "external/nlohmann/*.h",
  s.exclude_files = "test", "tests",
                    "wasm"

  s.public_header_files = "platforms/ios/dehancer-common/Classes/*.{h,hpp}"

  s.frameworks = "Foundation"
  s.libraries  = 'c++', 'ed25519cpp', 'base64cpp'

  s.requires_arc = true
  s.compiler_flags = '-Wno-format', '-x objective-c++', '-DNDEBUG', '-DUSE_DEC_FIXEDPOINT', '-DR128_STDC_ONLY'
  
  s.xcconfig = { 'GCC_PREPROCESSOR_DEFINITIONS' => 'CSA=1' , 'OTHER_CFLAGS' => '',
                 'HEADER_SEARCH_PATHS' => '"/usr/local/include" "${PODS_ROOT}" "${PODS_TARGET_SRCROOT}"/include',
                    'CLANG_CXX_LANGUAGE_STANDARD' => 'c++17',
                    'CLANG_CXX_LIBRARY' => 'libc++',
                    'LIBRARY_SEARCH_PATHS' => '"/usr/local/lib"'}
  
end
