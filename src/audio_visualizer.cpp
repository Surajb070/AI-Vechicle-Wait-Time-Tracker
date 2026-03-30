#include <gst/gst.h>
#include <iostream>

/*
 * GStreamer Audio Visualizer
 * This program uses GStreamer to create a visual representation (spectrum)
 * of an audio file.
 */

int main(int argc, char *argv[]) {
    GstElement *pipeline, *source, *demuxer, *decoder, *audioconvert, *visualizer, *videoconvert, *sink;
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;

    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <audio_file>" << std::endl;
        return -1;
    }

    /* Create the elements */
    source = gst_element_factory_make("filesrc", "source");
    demuxer = gst_element_factory_make("decodebin", "demuxer"); // decodebin handles demuxing and decoding
    audioconvert = gst_element_factory_make("audioconvert", "audioconvert");
    visualizer = gst_element_factory_make("wavescope", "visualizer"); // wavescope or spectrum
    videoconvert = gst_element_factory_make("videoconvert", "videoconvert");
    sink = gst_element_factory_make("autovideosink", "sink");

    /* Create the empty pipeline */
    pipeline = gst_pipeline_new("audio-visualizer-pipeline");

    if (!pipeline || !source || !demuxer || !audioconvert || !visualizer || !videoconvert || !sink) {
        std::cerr << "Not all elements could be created." << std::endl;
        return -1;
    }

    /* Build the pipeline */
    gst_bin_add_many(GST_BIN(pipeline), source, demuxer, audioconvert, visualizer, videoconvert, sink, NULL);

    /* Link source to demuxer */
    if (gst_element_link(source, demuxer) != TRUE) {
        std::cerr << "Source and demuxer could not be linked." << std::endl;
        gst_object_unref(pipeline);
        return -1;
    }

    /* Link remaining elements */
    if (gst_element_link_many(audioconvert, visualizer, videoconvert, sink, NULL) != TRUE) {
        std::cerr << "Remaining elements could not be linked." << std::endl;
        gst_object_unref(pipeline);
        return -1;
    }

    /* Set the input file */
    g_object_set(source, "location", argv[1], NULL);

    /* Connect to the pad-added signal for decodebin */
    g_signal_connect(demuxer, "pad-added", G_CALLBACK(+[](GstElement *src, GstPad *new_pad, gpointer data) {
        GstPad *sink_pad = gst_element_get_static_pad((GstElement *)data, "sink");
        if (gst_pad_is_linked(sink_pad)) {
            gst_object_unref(sink_pad);
            return;
        }
        GstCaps *new_pad_caps = gst_pad_get_current_caps(new_pad);
        GstStructure *new_pad_struct = gst_caps_get_structure(new_pad_caps, 0);
        const gchar *new_pad_type = gst_structure_get_name(new_pad_struct);
        if (g_str_has_prefix(new_pad_type, "audio/x-raw")) {
            if (gst_pad_link(new_pad, sink_pad) != GST_PAD_LINK_OK) {
                std::cerr << "Type is " << new_pad_type << " but link failed." << std::endl;
            }
        }
        if (new_pad_caps != NULL) gst_caps_unref(new_pad_caps);
        gst_object_unref(sink_pad);
    }), audioconvert);

    /* Start playing */
    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        std::cerr << "Unable to set the pipeline to the playing state." << std::endl;
        gst_object_unref(pipeline);
        return -1;
    }

    /* Wait until error or EOS */
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    /* Parse message */
    if (msg != NULL) {
        GError *err;
        gchar *debug_info;

        switch (GST_MESSAGE_TYPE(msg)) {
            case GST_MESSAGE_ERROR:
                gst_message_parse_error(msg, &err, &debug_info);
                std::cerr << "Error received from element " << GST_OBJECT_NAME(msg->src) << ": " << err->message << std::endl;
                std::cerr << "Debugging information: " << (debug_info ? debug_info : "none") << std::endl;
                g_clear_error(&err);
                g_free(debug_info);
                break;
            case GST_MESSAGE_EOS:
                std::cout << "End-Of-Stream reached." << std::endl;
                break;
            default:
                std::cerr << "Unexpected message received." << std::endl;
                break;
        }
        gst_message_unref(msg);
    }

    /* Free resources */
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    return 0;
}
