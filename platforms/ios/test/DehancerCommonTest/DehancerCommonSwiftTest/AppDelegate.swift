//
//  AppDelegate.swift
//  DehancerCommonSwiftTest
//
//  Created by denn nevera on 13/10/2019.
//  Copyright © 2019 Dehancer. All rights reserved.
//

import Cocoa
import DehancerCommon

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {


    func applicationDidFinishLaunching(_ aNotification: Notification) {
        // Insert code here to initialize your application
        
        let t = LicenseType.light
        
        Swift.print("... ", t.rawValue)
        
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }


}

