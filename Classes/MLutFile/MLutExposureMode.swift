//
//  TargetExposureMode.swift
//  Dehancer mLut Maker
//
//  Created by denn on 11.07.2018.
//  Copyright © 2018 Dehancer. All rights reserved.
//

import Foundation

public enum MLutExposureMode:Int {
    case under  = 0
    case normal = 1
    case over   = 2
    
    public init?(index:Int) {
        switch index {
        case 0:
            self = .under
        case 1:
            self = .normal
        case 2:
            self = .over
        default:
            return nil
        }
    }
    
    public var name:String {
        switch self {
        case .normal:
            return "Normal Exposure"
        case .over:
            return "Push +2EV"
        case .under:
            return "Pull -2EV"
        }
    }
    
    public var value:Float {
        switch self {
        case .normal:
            return 0
        case .over:
            return +2
        case .under:
            return -2
        }
    }
    
    static public let list:[MLutExposureMode] = [.under,.normal,.over]
    static public var count:Int { return list.count }
}
