# AvoGUI
A GUI library in c++ that is inspired by Google's Material Design guidelines. It was created because I wanted a high level GUI library with as much control as possible while keeping a logical and smooth workflow and doesn't make you write tons of unnecessary code, and that also has clear, readable and detailed documentation. It's basically my dream library.

## Getting started
All you need to do is add AvoGUI.hpp and AvoGUI.cpp to your project, and you're ready to start.

### Creating your GUI class
You first need to create a class that inherits AvoGUI::GUI. The GUI is made up of a view tree structure, where every view has a parent, some have one or more children and some have one or more siblings. The GUI is the only view that doesn't have a parent, and is the topmost view in the tree (depending on how you think about a tree, hmmm).

This is the structure of a GUI class. Every method that can be overridden by your GUI (assuming it only inherits AvoGUI::GUI) is shown here.
```cpp
class MyApplication : public AvoGUI::GUI
{
  MyApplication()
  {
    // Here, you can call create(). This creates the window and drawing context that the GUI relies on.
    // It is recommended to do any other initialization in createContent().
  }
  ~MyApplication()
  {
    // Do not forget() views yourself, they are cleaned up automatically if they have a parent.
  }
  
  //------------------------------
  // Events recieved from the window
  
  inline void handleMouseDown(const MouseEvent& p_event) override
  {
    // Calls the standard implementation that sends the event down to event listeners. It is recommended to do this since any
    // other views won't be able to react to mouse events otherwise. (note that this call is specific to this instance and is
    // not a static method, despite the syntax. Also, you do not need to specify the namespace when you do this, as seen.)
    GUI::handleMouseDown(p_event); 
    
    // If you for some reason want to react to mouse down events from your GUI, you can do it here.
    // This is called directly from the window, and the mouse coordinates in the event are relative
    // to the top-left corner of the window.
  }
  
  // Other events that you can override in your GUI are: handleMouseUp, handleDoubleClick, handleMouseMove, handleMouseScroll,
  // handleCharacterInput, handleKeyboardKeyDown and handleKeyboardKeyUp.
  
  //------------------------------
  
  inline void createContent() override
  {
    // Here you can create your views, set up your theme and initialize other things. This method has no default implementation.
  }
  inline void handleSizeChange() override
  {
    // This is called when the GUI has changed size due to window resizing. This method has no default implementation.
    // Note that this method belongs to the View class (which AvoGUI::GUI inherits) and can be implemented on any view.
  }
}
```

### Creating a custom view
First of all, what exactly is a view? A view is a rectangle that can draw itself, and is used to create GUI components. If you want your view to react to mouse interactions, inherit AvoGUI::MouseEventListener, register it with getGUI()->addMouseEventListener(this) and override the mouse event methods that you want to use. If you want your view to react to keyboard events, inherit AvoGUI::KeyboardEventListener, register it with getGUI()->addKeyboardEventListener(this) and override the keyboard event methods. Look at the documentation in AvoGUI.hpp for more information. 

This is the structure of a custom View class. Every method that can be overridden by your view (assuming it only inherits AvoGUI::View) is shown here.
```cpp
class MyView : public AvoGUI::View
{
  MyView(AvoGUI::View* p_parent, const Rectangle<float>& p_bounds) : 
    View(p_parent, p_bounds) // If you don't specify the bounds, it will be initialized with a size of (0, 0) and position of (0, 0)
  {
    // Here you can initialize the view, add child views, do whatever you want. To add a child view, all you need to do
    // is create a view with this view as the parent. Do not call addView() yourself.
  }
  ~MyView()
  {
    // Do not forget() child views yourself. They are released automatically.
  }
  
  //------------------------------
  
  inline void updateAnimations() override
  {
    // This method is used when you want to animate things. It is never called more frequently than a certain frequency
    // that you can change in the window (getGUI()->getWindow()->setAnimationTimerInterval(...)). To add the view to the
    // animation update queue, call queueAnimationUpdate(). If you would call queueAnimationUpdate() from this method
    // every time, animations would be updated constantly at the frequency set in the window. This system is efficient
    // since you only need to call queueAnimationUpdate() when you know that your animation isn't done.
  }
  
  inline void handleSizeChange() override
  {
    // This is called when the size of the view has changed. You can update your layout here. There is no standard 
    // implementation.
  }
  
  //------------------------------
  
  inline void draw(AvoGUI::DrawingContext* p_drawingContext) override
  {
    // Draw your view here! If this view has children, do not draw them yourself. They are always drawn by the GUI, on
    // top of what is drawn here. Now, how do you decide that you want your view to get drawn? Do NOT call this method
    // directly, instead call invalidate() or getGUI()->invalidateRect(...) to tell the system that a portion of the
    // GUI has been updated and wants to be redrawn.
  }
  inline void draw(AvoGUI::DrawingContext* p_drawingContext, const Rectangle<float>& p_targetRectangle) override
  {
    // This can be implemented instead of the other draw method if you want to know the rectangle that is going to be drawn. 
    // The default implementation only calls draw(AvoGUI::DrawingContext* p_drawingContext).
  }
  
  inline void drawUnclipped(AvoGUI::DrawingContext* p_drawingContext) override
  {
    // Here, you can draw anything that you do not want to get affected by any view clipping. Anything drawn here
    // is on top of what was drawn in draw().
  }
  inline void drawUnclipped(AvoGUI::DrawingContext* p_drawingContext, const Rectangle<float>& p_targetRectangle) override
  {
    // Default implementation only calls drawUnclipped(AvoGUI::DrawingContext* p_drawingContext).
  }
}
```

Look around in the documentation in AvoGUI.hpp for more information about anything.

## Compatible platforms
The goal is to support Windows, Linux and MacOS. Currently, only Windows is supported.

## Development state
The project is constantly morphing and taking shape to become the ideal GUI library, which means big changes can happen anytime as long as they improve for example workflow, consistency or efficiency. 

## Contributing
Don't be afraid to submit issues about the little things! All issues are very welcome, and they help the development of the library a lot. Pull requests are also welcome.
