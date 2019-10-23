
#ifndef EGT_DETAIL_CAMERA_GSTIMPL_H
#define EGT_DETAIL_CAMERA_GSTIMPL_H

#include "egt/camera.h"
#include <gst/gst.h>
#include <string>
#include <thread>

using namespace std;

namespace egt
{
inline namespace v1
{
namespace detail
{

class CameraImpl
{
public:
    CameraImpl(CameraWindow& interface, const Rect& rect,
               const std::string& device, bool useKmssink);

    virtual void draw(Painter& painter, const Rect& rect);

    virtual bool start();

    virtual void stop();

    virtual void scale(float scalex, float scaley);

    std::string error_message() const;

    virtual ~CameraImpl();

protected:
    CameraWindow& m_interface;
    std::string m_devnode;
    GstElement* m_pipeline{nullptr};
    GstElement* m_appsink{nullptr};
    GstSample* m_camerasample{nullptr};
    std::string m_err_message;
    Rect m_rect;
    bool m_usekmssink;
    GMainLoop* m_gmainLoop{nullptr};
    std::thread m_gmainThread;

    static GstFlowReturn on_new_buffer(GstElement* elt, gpointer data);

    static gboolean bus_callback(GstBus* bus, GstMessage* message, gpointer data);
};

} //End of detail

} //End of v1

} //End of egt


#endif
