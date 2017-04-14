<a href="http://www.wrld3d.com/">
    <img src="http://cdn2.eegeo.com/wp-content/uploads/2017/04/WRLD_Blue.png"  align="right" height="80px" />
</a>

# WRLD Cardboard Demo

![WRLD](http://cdn2.eegeo.com/wp-content/uploads/2017/04/screenselection01.png)

* [Support](#support)
* [Getting Started](#getting-started)
    * [WRLD API Key](#eegeo-api-key)
* [WRLD SDK Documentation](#eegeo-sdk-documentation)
* [Cardboard API Documentation](#cardboard-api-documentation)
    * [Jump Points](#jump-points)
        * [Creating a Jump Point](#creating-a-jump-point)  
        * [Removing a Jump Point](#removing-a-jump-point)
    * [Gaze UI](#gaze-ui)
        * [Creating a Button](#creating-a-button)
    * [Changing Gaze Icons](#changing-gaze-icons)
    * [Icon Sheet](#icon-sheet)
* [License](#license)

This example app showcases the [WRLD SDK's](http://www.wrld3d.com/) integration with Google Cardboard for creating map based VR experiences.

### What does the API have to offer?

* A full 3D, virtual reality map of the world.
* Head tracking and magnet trigger usage
* Lens distortion integration with Google Cardboard QR Code based profile selector
* Gaze UI - The ability to add buttons that the user can select by gazing at them.
* Jump Points - Adding points in the world that the user can jump to by gazing at them.

## Support

If you have any questions, bug reports, or feature requests, feel free to submit to the [issue tracker](https://github.com/wrld3d/cardboard-vr-integration/issues) for this repository.

## Getting Started

This example app demonstrates the use of WRLD's Cardboard API that allows developers to create VR experiences on Android.

This section will walk you through the process of getting up and running quickly on each platform.

1.  Clone this repo: `git clone https://github.com/wrld3d/cardboard-vr-integration`
2.  Obtain an [WRLD API key](https://www.wrld3d.com/developers/apikeys) and place it in the [app_config.json](https://github.com/wrld3d/cardboard-vr-demo/blob/master/android/assets/ApplicationConfigs/app_config.json#L7) file.
3.  Choose a platform from the below table to see detailed instructions for building the app.

Platform                                        | Languages         
------------------------------------------------|-------------------
[Getting started on Android](/android#readme)   | C++, Java         

### WRLD API Key 

In order to use the WRLD 3D Maps SDK, you must sign up for a free developer account at https://www.wrld3d.com/developers. After signing up, you'll be able to create an [API key](https://www.wrld3d.com/developers/apikeys) for your apps. 

To run this example app, you must place the API key in the [app_config.json](https://github.com/wrld3d/cardboard-vr-demo/blob/master/android/assets/ApplicationConfigs/app_config.json#L7) file.

## WRLD SDK Documentation

See the [WRLD API reference](http://cdn1.wrld3d.com/docs/mobile-sdk/namespaces.html) for documentation on the individual SDK types.

## Cardboard API Documentation

Following are some example of how to use specific features of the Gaze UI API.

### Jump Points

#### Creating a Jump Point

In order to create a jump point you need to create an object of the JumpPoint class

```c++
//Following attributes are required for creation of jump points.

//A unique id for jump point
Eegeo::UI::JumpPoints::TJumpPointId jumpPointID = 1;
//The latitude, longitude and altitude for the jump point
Eegeo::Space::LatLongAltitude jumpPointPosition = Eegeo::Space::LatLongAltitude::FromDegrees(56.459935, -2.974200, 250);
//Path to the image file to be used as jump point icon
std::string jumpPointIcon = "mesh_example/quadrants.png";
//The size of the jump point
Eegeo::v2 jumpPointSize = Eegeo::v2(50,50);

//Create an object of JumpPoint that will be added to the jump point repository
Eegeo::UI::JumpPoints::JumpPoint* myJumpPoint = 
Eegeo_NEW(Eegeo::UI::JumpPoints::JumpPoint) ( jumpPointID
                                            , jumpPointPosition
                                            , jumpPointIcon
                                            , jumpPointSize);

//Add the jump point to repository so it can start drawing on screen at the specified location
m_JumpPointsModule->GetRepository().AddJumpPoint(myJumpPoint);
```

There are also optional parameters that can be used. These include the min and max UVs for the jump point texture and custom user data.

```c++
//Create and object of JumpPoint that will be added to the jump point repository
Eegeo::UI::JumpPoints::JumpPoint* myJumpPoint = 
Eegeo_NEW(Eegeo::UI::JumpPoints::JumpPoint) ( jumpPointID
                                            , jumpPointPosition
                                            , jumpPointIcon
                                            , jumpPointSize
                                            , minUVs
                                            , maxUVs
                                            , cutsomUserData);
```


#### Removing a Jump Point

To remove a jump point, simply remove the jump point object from the repository.

```c++
m_JumpPointsModule->GetRepository().RemoveJumpPoint(myJumpPoint);
```

### Gaze UI

#### Creating a Button

In order to create a gaze button we need a callback that will be called when button is interacted with. Assuming that you have a class named MyExampleClass with a method MyExampleMethod that needs to be triggered with a button

```c++
class MyExampleClass
{
private:
    //The callback to be passed to the button
    Eegeo::Helpers::TCallback0<MyExampleClass>* m_ClickCallback;
    //Button pointer
    Eegeo::UI::UIImageButton *m_UIButton;

public:
    //Method that will be called when the button is triggered
    void MyExampleMethod()
    {
        //Log to show that gaze interaction with button was completed.
        EXAMPLE_LOG("Button was gazed upon");
    }
    
    void CreateButton(Eegeo::EegeoWorld& world, Eegeo::UI::IUIInteractionObservable& p_InteractionModule, Eegeo::UI::IUIQuadFactory& p_QuadFactory)
    {
        //Initializing callback for button
        m_ClickCallback =
        Eegeo_NEW(Eegeo::Helpers::TCallback0<MyExampleClass>)(this, &MyExampleClass::MyExampleMethod);
        
        //Position of button
        Eegeo::Space::LatLongAltitude buttonPosition = Eegeo::Space::LatLongAltitude::FromDegrees(56.459935, -2.974200, 250);
        //Path of sprite sheet for button
        std::string buttonIcon = "mesh_example/PinIconTexturePage.png";
        //Size of our sprite sheet
        Eegeo::v2 size(4,4);
        int spriteID = 1; //Sprite id start from 0 from bottom left and end at top right
        //Calculating UVs for the spriteID that we want in sprite sheet
        Eegeo::v2 outUVMin;
        Eegeo::v2 outUVMax;
        Eegeo::UI::CalculateUV(size, spriteID, outUVMin, outUVMax);
        //Size of button
        Eegeo::v2 buttonSize = Eegeo::v2(50,50);
        
        //Create a button object
        m_UIButton = Eegeo_NEW(Eegeo::UI::UIImageButton)(
                                                         p_QuadFactory.CreateUIQuad(buttonIcon, buttonSize, outUVMin, outUVMax),
                                                         buttonSize,
                                                         buttonPosition.ToECEF(),
                                                         *m_ClickCallback);
        
        //Register the button with interaction module so it can start receiving gaze events
        p_InteractionModule.RegisterInteractableItem(m_UIButton);
    }
};
```


### Changing Gaze Icons

In order to change the interaction animation, follow these steps:
* Replace the loading sprite sheet with the your custom image by replacing [gaze_loader.png](https://github.com/wrld3d/cardboard-vr-integration/blob/master/android/assets/mesh_example/gaze_loader.png).
* Update the following things in the code below located in constructor of [ExampleApp.cpp](https://github.com/wrld3d/cardboard-vr-integration/blob/master/src/ExampleApp.cpp#L231)
    * Change the `spriteSheetGridSize` according to your sprite sheet.
    * Change the last parameter `framesPerSecond` to change the speed of animation.

```c++
...
m_GazeProgress = Eegeo_NEW(Eegeo::UI::UIAnimatedSprite)(
                                m_QuadFactory->CreateUIQuad(spriteSheetPath, dimension)
                                , clickCallback
                                , dimension
                                , spriteSheetGridSize
                                , framesPerSecond
                                );
...    
```

In order to change the gaze icon replace the gaze icon with the your custom image by replacing [gaze_point.png](https://github.com/wrld3d/cardboard-vr-integration/blob/master/android/assets/mesh_example/gaze_point.png).

## Icon Sheet

![Icon Sheet](https://github.com/eegeo/cardboard-vr-integration/blob/master/android/assets/mesh_example/PinIconTexturePage.png)

This is a default 6x6 grid that is being used by the sdk to draw the icons. In order to use different icons either add a new sheet or simply replace an icon from the placeholders provided and use their UVs to draw the appropriate image.

## License

The WRLD 3D Maps Cardboard Demo is released under the Simplified BSD License. See the [LICENSE.md](https://github.com/wrld3d/cardboard-vr-integration/blob/master/LICENSE.md) file for details.
