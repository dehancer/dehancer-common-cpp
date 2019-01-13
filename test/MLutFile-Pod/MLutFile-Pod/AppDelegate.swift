//
//  AppDelegate.swift
//  MLutFile-Pod
//
//  Created by denn on 12/01/2019.
//  Copyright © 2019 Dehancer. All rights reserved.
//

import Cocoa
import DehancerMLutFile

@NSApplicationMain
class AppDelegate: NSObject, NSApplicationDelegate {

    @IBOutlet weak var window: NSWindow!

    let luTypeList = MLutFilmType.availableList
    let cap = MLutColorType.color.caption

    func applicationDidFinishLaunching(_ aNotification: Notification) {
        // Insert code here to initialize your application
    }

    func applicationWillTerminate(_ aNotification: Notification) {
        // Insert code here to tear down your application
    }


}

