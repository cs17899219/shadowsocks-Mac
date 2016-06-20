//
//  ServerProfileManager.swift
//  ShadowsocksX-NG
//
//  Created by 邱宇舟 on 16/6/6.
//  Copyright © 2016年 qiuyuzhou. All rights reserved.
//

import Cocoa

class AppProfileManager {
    
    static let instance:AppProfileManager = AppProfileManager()
    
    var profiles:[AppProfile] = [AppProfile]()
    
    private var _activePID: String!;
    
    var activeProfileId: String? {
        set {
            NSUserDefaults.standardUserDefaults().setObject(newValue, forKey: "ActiveServerProfileId")
            _activePID = newValue;
        }
        get {
            return _activePID
        }
    }
    
    private init() {
        let defaults = NSUserDefaults.standardUserDefaults()
        if let _profiles = defaults.arrayForKey("ServerProfiles") {
            for _profile in _profiles {
                let profile = AppProfile.fromDictionary(_profile as! [String : AnyObject])
                profiles.append(profile)
            }
        }
        activeProfileId = defaults.stringForKey("ActiveServerProfileId")
    }
    
    func save() {
        let defaults = NSUserDefaults.standardUserDefaults()
        
        // Save Profiles
        var _profiles = [AnyObject]()
        for profile in profiles {
            if profile.isValid() {
                _profiles.append(profile.toDictionary())
            }
        }
        defaults.setObject(_profiles, forKey: "ServerProfiles")
        
        // Deal with ActivedProfile
        if getActivedProfile() == nil {
            activeProfileId = nil
        }
        
        if activeProfileId != nil {
            defaults.setObject(activeProfileId, forKey: "ActiveServerProfileId")
        } else {
            defaults.removeObjectForKey("ActiveServerProfileId")
        }
    }
    
    func getActivedProfile() -> AppProfile? {
        if let uuid = activeProfileId {
            for profile in profiles {
                if (profile.uuid == uuid) {
                    return profile
                }
            }
            return nil
        } else {
            return nil
        }
    }
}
