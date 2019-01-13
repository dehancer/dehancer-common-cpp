Pod::Spec.new do |s|
    s.name         = 'DehancerMLutFile'
    s.version      = '0.1.1'
    s.license      = { :type => 'MIT', :file => 'LICENSE' }
    s.author       = { 'Denn Nevera' => 'https://imagemetalling.wordpress.com/' }
    s.homepage     = 'http://www.dehancer.com/'
    s.summary      = 'Dehancer Multi Color Lookup Table File format'
    s.description  = 'Dehancer Multi Color Lookup Table File format. MLut is format describes real film colors LUT. MLut contains difference profiles...'
    
    s.source       = { :git => 'https://github.com/dehancer/DehancerMLutFile', :tag => s.version}
    
    s.osx.deployment_target = "10.12"
    s.swift_version = "4.2"

    s.source_files  = 'Classes/**/*.{h,m,swift}'
    s.public_header_files = 'Classes/**/*.h'
    s.dependency  'IMProcessingXMP'
    s.dependency  'IMProcessing'
    s.dependency  'Surge'
    s.dependency  'CryptoSwift'

    s.requires_arc = true

    #s.osx.pod_target_xcconfig = { 'LIBRARY_SEARCH_PATHS' => '$(inherited) $(PODS_ROOT)/IMProcessingXMP/lib-macos/release  $(PODS_TARGET_SRCROOT)/../${POD_NAME}/lib-macos/release' }
end
