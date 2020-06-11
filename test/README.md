# EGT Test Suite

This directory contains a collection of EGT unit tests built and run using the
[Googletest](https://github.com/google/googletest) framework.

If you want to write or update tests, the
[Googletest Primer](https://github.com/google/googletest/blob/master/googletest/docs/primer.md)
is a very good starting point.

## Running Tests

Running the tests is as simple as running

```
make check
```

However, some media assets are required by tests.  Those assets are either in
the egt repository itself or the egt-media tarball used by build systems.  You
can use the `EGT_SEARCH_PATH` environment variable to specify those paths.

```
EGT_SEARCH_PATH=~/egt-media/examples/video/:~/egt/examples/audioplayer/ make check
```

Sometimes it is burdensome to use the build system to build and run the test
together. You can just build the test program by running:

```
cd test
make test
```

Then, you can run the result directly with:

```
./test
```

or by specifying where some assets are:

```
EGT_SEARCH_PATH=~/egt-media/examples/video/:~/egt/examples/audioplayer/ ./test
```

Some tests may be disabled by default.  To enable those tests add the following
command line parameter to your invocation.

```
./test --gtest_also_run_disabled_tests
```

The test program supports various command line arguments which can be see by
running

```
./test --help
```

You can get the list of available tests, for example at the time of writing:

```
$ ./test --gtest_list_tests
Math.
  CompareFloat
  CompareDouble
Color.
  Basic
TextBox.
  Basic
TextBoxFixed.
  Basic
Screen.
  DamageAlgorithm
Canvas.
  Basic
Geometry.
  Basic
  Points
  Rects
  Sizes
  Arcs
  Circless
Widgets/0.  # TypeParam = egt::v1::AnalogMeterType<int>
  Properties
Widgets/1.  # TypeParam = egt::v1::Button
  Properties
Widgets/2.  # TypeParam = egt::v1::ImageButton
  Properties
Widgets/3.  # TypeParam = egt::v1::CheckBox
  Properties
Widgets/4.  # TypeParam = egt::v1::CircleWidget
  Properties
Widgets/5.  # TypeParam = egt::v1::Label
  Properties
Widgets/6.  # TypeParam = egt::v1::ImageLabel
  Properties
Widgets/7.  # TypeParam = egt::v1::ListBox
  Properties
Widgets/8.  # TypeParam = egt::v1::RadioBox
  Properties
Widgets/9.  # TypeParam = egt::v1::TextBox
  Properties
Widgets/10.  # TypeParam = egt::v1::ComboBox
  Properties
Widgets/11.  # TypeParam = egt::v1::Slider
  Properties
Widgets/12.  # TypeParam = egt::v1::BoxSizer
  Properties
Widgets/13.  # TypeParam = egt::v1::VerticalBoxSizer
  Properties
Widgets/14.  # TypeParam = egt::v1::HorizontalBoxSizer
  Properties
Widgets/15.  # TypeParam = egt::v1::Notebook
  Properties
Widgets/16.  # TypeParam = egt::v1::StaticGrid
  Properties
Widgets/17.  # TypeParam = egt::v1::Scrollwheel
  Properties
Widgets/18.  # TypeParam = egt::v1::ProgressBarType<int>
  Properties
Widgets/19.  # TypeParam = egt::v1::SpinProgressType<int>
  Properties
Widgets/20.  # TypeParam = egt::v1::LevelMeterType<int>
  Properties
Widgets/21.  # TypeParam = egt::v1::AnalogMeterType<int>
  Properties
Widgets/22.  # TypeParam = egt::v1::CircleWidget
  Properties
Widgets/23.  # TypeParam = egt::v1::LineWidget
  Properties
Widgets/24.  # TypeParam = egt::v1::RectangleWidget
  Properties
WidgetFlags.
  Basic
AlignFlags.
  Basic
Network.
  URI
ButtonWidgets/0.  # TypeParam = egt::v1::Button
  TestValueRange
ButtonWidgets/1.  # TypeParam = egt::v1::RadioBox
  TestValueRange
ButtonWidgets/2.  # TypeParam = egt::v1::ImageButton
  TestValueRange
ButtonWidgets/3.  # TypeParam = egt::v1::CheckButton
  TestValueRange
ButtonWidgets/4.  # TypeParam = egt::v1::CheckBox
  TestValueRange
ImageButtonTest.
  ImageButton
ValueRangeWidgets/0.  # TypeParam = egt::v1::AnalogMeterType<int>
  TestValueRange
ValueRangeWidgets/1.  # TypeParam = egt::v1::ProgressBarType<int>
  TestValueRange
ValueRangeWidgets/2.  # TypeParam = egt::v1::SpinProgressType<int>
  TestValueRange
ValueRangeWidgets/3.  # TypeParam = egt::v1::LevelMeterType<int>
  TestValueRange
ValueRangeWidgets/4.  # TypeParam = egt::v1::Slider
  TestValueRange
FloatWidgets/0.  # TypeParam = egt::v1::AnalogMeterType<float>
  TestValueRangeF
FloatWidgets/1.  # TypeParam = egt::v1::ProgressBarType<float>
  TestValueRangeF
FloatWidgets/2.  # TypeParam = egt::v1::SpinProgressType<float>
  TestValueRangeF
FloatWidgets/3.  # TypeParam = egt::v1::LevelMeterType<float>
  TestValueRangeF
ComboBoxWidgetTestGroup/ComboBoxWidgetTest.
  TestWidget/0  # GetParam() = 0
  TestWidget/1  # GetParam() = 1
  TestWidget/2  # GetParam() = 2
  TestWidget/3  # GetParam() = 3
FormTestGroup/FormTest.
  TestWidget/0  # GetParam() = 0
  TestWidget/1  # GetParam() = 1
FrameTestGroup/FrameTest.
  TestWidget/0  # GetParam() = 1
  TestWidget/1  # GetParam() = 2
  TestWidget/2  # GetParam() = 4
StaticGridTestGroup/StaticGridTest.
  TestWidget/0  # GetParam() = (1, 1)
  TestWidget/1  # GetParam() = (1, 2)
  TestWidget/2  # GetParam() = (2, 1)
  TestWidget/3  # GetParam() = (2, 2)
SelectableGridTestGroup/SelectableGridTest.
  TestWidget/0  # GetParam() = (1, 1)
  TestWidget/1  # GetParam() = (1, 2)
  TestWidget/2  # GetParam() = (1, 3)
  TestWidget/3  # GetParam() = (1, 4)
  TestWidget/4  # GetParam() = (2, 1)
  TestWidget/5  # GetParam() = (2, 2)
  TestWidget/6  # GetParam() = (2, 3)
  TestWidget/7  # GetParam() = (2, 4)
  TestWidget/8  # GetParam() = (3, 1)
  TestWidget/9  # GetParam() = (3, 2)
  TestWidget/10  # GetParam() = (3, 3)
  TestWidget/11  # GetParam() = (3, 4)
  TestWidget/12  # GetParam() = (4, 1)
  TestWidget/13  # GetParam() = (4, 2)
  TestWidget/14  # GetParam() = (4, 3)
  TestWidget/15  # GetParam() = (4, 4)
ListBoxWidgetTestGroup/ListBoxWidgetTest.
  TestWidget/0  # GetParam() = 0
  TestWidget/1  # GetParam() = 1
  TestWidget/2  # GetParam() = 2
  TestWidget/3  # GetParam() = 3
NoteBookTestGroup/NoteBookTest.
  TestWidget/0  # GetParam() = 0
  TestWidget/1  # GetParam() = 1
DISABLED_ScrollwheelTestGroup/ScrollwheelTest.
  Test1/0  # GetParam() = (0, 0)
  Test1/1  # GetParam() = (0, 1)
  Test1/2  # GetParam() = (1, 0)
  Test1/3  # GetParam() = (1, 1)
  Test1/4  # GetParam() = (2, 0)
  Test1/5  # GetParam() = (2, 1)
  Test1/6  # GetParam() = (3, 0)
  Test1/7  # GetParam() = (3, 1)
  Test1/8  # GetParam() = (4, 0)
  Test1/9  # GetParam() = (4, 1)
  Test1/10  # GetParam() = (5, 0)
  Test1/11  # GetParam() = (5, 1)
SizerTestGroup/SizerTest.
  TestWidget/0  # GetParam() = 1
  TestWidget/1  # GetParam() = 2
  TestWidget/2  # GetParam() = 4
  TestWidget/3  # GetParam() = 8
SliderTestGroup/SliderTest.
  CreateSlider/0  # GetParam() = 1
  CreateSlider/1  # GetParam() = 2
  CreateSlider/2  # GetParam() = 4
  CreateSlider/3  # GetParam() = 8
  CreateSlider/4  # GetParam() = 16
  CreateSlider/5  # GetParam() = 32
  CreateSlider/6  # GetParam() = 64
ViewTestGroup/ViewTest.
  TestWidget/0  # GetParam() = (0, 0)
  TestWidget/1  # GetParam() = (0, 1)
  TestWidget/2  # GetParam() = (0, 2)
  TestWidget/3  # GetParam() = (1, 0)
  TestWidget/4  # GetParam() = (1, 1)
  TestWidget/5  # GetParam() = (1, 2)
  TestWidget/6  # GetParam() = (2, 0)
  TestWidget/7  # GetParam() = (2, 1)
  TestWidget/8  # GetParam() = (2, 2)
AudioPlayerGroup/AudioPlayerTest.
  AudioPlayer/0  # GetParam() = ""
  AudioPlayer/1  # GetParam() = "file:/vgfweyquwfudqwfufcdqwcudqw.kc"
  AudioPlayer/2  # GetParam() = "file:concerto.mp3"
VideoWindowTestGroup/VideoWidgetTest.
  VideoWidget/0  # GetParam() = ""
  VideoWidget/1  # GetParam() = "file:ewfgfewyyewfyfewylfew"
  VideoWidget/2  # GetParam() = "file:microchip_corporate_mpeg2.avi"
DISABLED_CreateVideoWindowGroup/CreateVideoWindowTest.
  DefaultVideoWindow/0  # GetParam() = (1, 1)
  DefaultVideoWindow/1  # GetParam() = (1, 2)
  DefaultVideoWindow/2  # GetParam() = (1, 4)
  DefaultVideoWindow/3  # GetParam() = (1, 8)
  DefaultVideoWindow/4  # GetParam() = (1, 16)
  DefaultVideoWindow/5  # GetParam() = (2, 1)
  DefaultVideoWindow/6  # GetParam() = (2, 2)
  DefaultVideoWindow/7  # GetParam() = (2, 4)
  DefaultVideoWindow/8  # GetParam() = (2, 8)
  DefaultVideoWindow/9  # GetParam() = (2, 16)
  DefaultVideoWindow/10  # GetParam() = (3, 1)
  DefaultVideoWindow/11  # GetParam() = (3, 2)
  DefaultVideoWindow/12  # GetParam() = (3, 4)
  DefaultVideoWindow/13  # GetParam() = (3, 8)
  DefaultVideoWindow/14  # GetParam() = (3, 16)
  DefaultVideoWindow/15  # GetParam() = (4, 1)
  DefaultVideoWindow/16  # GetParam() = (4, 2)
  DefaultVideoWindow/17  # GetParam() = (4, 4)
  DefaultVideoWindow/18  # GetParam() = (4, 8)
  DefaultVideoWindow/19  # GetParam() = (4, 16)
  DefaultVideoWindow/20  # GetParam() = (5, 1)
  DefaultVideoWindow/21  # GetParam() = (5, 2)
  DefaultVideoWindow/22  # GetParam() = (5, 4)
  DefaultVideoWindow/23  # GetParam() = (5, 8)
  DefaultVideoWindow/24  # GetParam() = (5, 16)
  DefaultVideoWindow/25  # GetParam() = (6, 1)
  DefaultVideoWindow/26  # GetParam() = (6, 2)
  DefaultVideoWindow/27  # GetParam() = (6, 4)
  DefaultVideoWindow/28  # GetParam() = (6, 8)
  DefaultVideoWindow/29  # GetParam() = (6, 16)
  DefaultVideoWindow/30  # GetParam() = (7, 1)
  DefaultVideoWindow/31  # GetParam() = (7, 2)
  DefaultVideoWindow/32  # GetParam() = (7, 4)
  DefaultVideoWindow/33  # GetParam() = (7, 8)
  DefaultVideoWindow/34  # GetParam() = (7, 16)
  DefaultVideoWindow/35  # GetParam() = (8, 1)
  DefaultVideoWindow/36  # GetParam() = (8, 2)
  DefaultVideoWindow/37  # GetParam() = (8, 4)
  DefaultVideoWindow/38  # GetParam() = (8, 8)
  DefaultVideoWindow/39  # GetParam() = (8, 16)
  DefaultVideoWindow/40  # GetParam() = (9, 1)
  DefaultVideoWindow/41  # GetParam() = (9, 2)
  DefaultVideoWindow/42  # GetParam() = (9, 4)
  DefaultVideoWindow/43  # GetParam() = (9, 8)
  DefaultVideoWindow/44  # GetParam() = (9, 16)
  DefaultVideoWindow/45  # GetParam() = (10, 1)
  DefaultVideoWindow/46  # GetParam() = (10, 2)
  DefaultVideoWindow/47  # GetParam() = (10, 4)
  DefaultVideoWindow/48  # GetParam() = (10, 8)
  DefaultVideoWindow/49  # GetParam() = (10, 16)
CameraWindowTestGroup/CameraWindowTest.
  CameraWidget/0  # GetParam() = "/dev/video0"
```

With this information, instead of running all tests, you can selectively run
tests.  So, to run just the tests in the `Math` suite, you can run

```
$ ./test --gtest_filter=Math.*
Note: Google Test filter = Math.*
[==========] Running 2 tests from 1 test suite.
[----------] Global test environment set-up.
[----------] 2 tests from Math
[ RUN      ] Math.CompareFloat
[       OK ] Math.CompareFloat (0 ms)
[ RUN      ] Math.CompareDouble
[       OK ] Math.CompareDouble (0 ms)
[----------] 2 tests from Math (0 ms total)

[----------] Global test environment tear-down
[==========] 2 tests from 1 test suite ran. (0 ms total)
[  PASSED  ] 2 tests.
```

In the event a test is failing, the `test.log` file generated when running the
test program has more information.

If a GUI to run the tests is more to your liking,
[gtest-runner](https://github.com/nholthaus/gtest-runner) is pretty handy.
