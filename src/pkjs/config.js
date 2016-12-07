module.exports = [
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Personalization Settings"
      },
      {
        "id": "classYear",
        "type": "input",
        "messageKey": "ClassYear",
        "label": "Class Year",
        "defaultValue": "",
        "attributes": {
          "limit": "4",
          "type": "number",
          "placeholder": "eg: 2005"
        },
        "description": "The class year that you want displayed on watchface modes that support it."
      },
      {
        "id": "cadetCompany",
        "type": "select",
        "messageKey": "CadetCompany",
        "label": "Cadet Company",
        "defaultValue": "none",
        "description": "Choose a cadet company if you want it to be shown as a watchface mode. If this is left as 'none' no cadet company face will be shown.",
        "options": [
          {
            "label": "None",
            "value": "none"
          },
          {
            "label": "First Battalion",
            "value": [
              {
                "label": "Alpha",
                "value": "a"
              },
              {
                "label": "Bravo",
                "value": "b"
              },
              {
                "label": "Charlie",
                "value": "c"
              },
              {
                "label": "Delta",
                "value": "d"
              },
            ]
          }, // end first bat
          {
            "label": "Second Battalion",
            "value": [
              {
                "label": "Echo",
                "value": "e"
              },
              {
                "label": "F-Troop",
                "value": "f"
              },
              {
                "label": "Golf",
                "value": "g"
              },
              {
                "label": "Hotel",
                "value": "h"
              },
              {
                "label": "Regimental Band",
                "value": "rb"
              },
            ]
          }, // end second bat
          {
            "label": "Third Battalion",
            "value": [
              {
                "label": "India",
                "value": "i"
              },
              {
                "label": "Lima",
                "value": "l"
              },
              {
                "label": "Kilo",
                "value": "k"
              },
              {
                "label": "Mike",
                "value": "m"
              },              
            ]
          }, // end third bat          
          {
            "label": "Fourth Battalion",
            "value": [
              {
                "label": "November",
                "value": "n"
              },
              {
                "label": "Oscar",
                "value": "o"
              },
              {
                "label": "Romeo",
                "value": "r"
              },
              {
                "label": "Tango",
                "value": "t"
              },
            ]
          }, // end fourth bat
          {
            "label": "Fifth Battalion",
            "value": [
              {
                "label": "Palmetto Battery",
                "value": "pb"
              },
              {
                "label": "Sierra",
                "value": "s"
              },
              {
                "label": "Victor",
                "value": "v"
              },
              {
                "label": "Whiskey",
                "value": "w"
              },
            ]
          }, // end fifth bat          
        ] // end options
      },
    ]
  },
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Watchface Settings"
      },
      {
        "id": "watchfaceRotation",
        "type": "checkboxgroup",
        "messageKey": "WatchfaceRotation",
        "label": "Watchface Rotation",
        "defaultValue": [true, true, true, true, true, true, true],
        "options": ["Ring", "Cadet Company", "Citadel Logo", "Big Red", "Bulldog", "CAA Logo", "Citadel Seal"],
        "description": "Enabled modes will be added into the rotation and cycled to the next mode at the chosen interval"
      },
      {
        "id": "modeChangeInterval",
        "type": "slider",
        "messageKey": "ModeChangeInterval",
        "label": "Mode Change Interval",
        "defaultValue": 10,
        "min": 1,
        "max": 60,
        "description": "The watchface mode will cycle to the next mode on the selected interval"
      },
      {
        "id": "modeChangeShake",
        "type": "toggle",
        "messageKey": "ModeChangeShake",
        "label": "Change Modes on Shake",
        "defaultValue": false,
        "description": "If this is selected then when the watch is shaken or tapped the watchface will change"
      }      
    ] // end items
  }, // end section  
  {
    "type": "section",
    "items": [
      {
        "type": "heading",
        "defaultValue": "Vibration Settings"
      },
      {
        "id": "hourVibration",
        "type": "toggle",
        "messageKey": "HourVibration",
        "label": "Vibrate on the Hour",
        "defaultValue": true
      },
      {
        "id": "vibrateType",
        "type": "select",
        "messageKey": "HourVibrationType",
        "label": "Hourly Vibration Type",
        "defaultValue": 2,
        "description": "Short, long and double use built-in vibration types. The pattern type will vibrate the hour. A long vibration is equal to five hours so that seven o'clock will be one long vibration followed by two short.",
        "options": [
          {
            "label": "Short",
            "value": 1
          },
          {
            "label": "Long",
            "value": 2
          },
          {
            "label": "Double",
            "value": 3
          },
          {
            "label": "Pattern",
            "value": 4
          },
        ]
      },
      {
        "id": "quarterHourVibration",
        "type": "toggle",
        "messageKey": "QuarterHourVibration",
        "label": "Vibrate on Quarter Hour",
        "defaultValue": false
      },
      {
        "id": "vibrateType",
        "type": "select",
        "messageKey": "QuarterHourVibrationType",
        "label": "Quarter Hour Vibration Type",
        "defaultValue": 1,
        "description": "Short, long and double use built-in vibration types. The fourth type will vibrate one time for the one quarter after, twice for two quarters, and three times for three quarters.",
        "options": [
          {
            "label": "Short",
            "value": 1
          },
          {
            "label": "Long",
            "value": 2
          },
          {
            "label": "Double",
            "value": 3
          },
          {
            "label": "Quarter Count",
            "value": 4
          },
        ]
      }          
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];