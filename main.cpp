#include <QCoreApplication>
#include <iostream>
#include <Common/exampleHelper.h>
#include <mvIMPACT_CPP/mvIMPACT_acquire.h>
#include <mvDisplay/Include/mvIMPACT_acquire_display.h>

using namespace std;
using namespace mvIMPACT::acquire;
using namespace mvIMPACT::acquire::display;

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DeviceManager devMgr;
    Device* pDev = getDeviceFromUserInput( devMgr );
    if( !pDev )
    {
        cout << "Unable to continue!" << endl
             << "Press [ENTER] to end the application" << endl;
        cin.get();
        return 0;
    }

    try
    {
        pDev->open();
    }
    catch( const ImpactAcquireException& e )
    {
        // this e.g. might happen if the same device is already opened in another process...
        cout << "An error occurred while opening the device(error code: " << e.getErrorCode() << ")." << endl
             << "Press [ENTER] to end the application" << endl;
        cin.get();
        return 0;
    }

    FunctionInterface fi( pDev );

    // send a request to the default request queue of the device and wait for the result.
    fi.imageRequestSingle();
    manuallyStartAcquisitionIfNeeded( pDev, fi );
    // Define the Image Result Timeout (The maximum time allowed for the Application
    // to wait for a Result). Infinity value:-1
    const int iMaxWaitTime_ms = -1;   // USB 1.1 on an embedded system needs a large timeout for the first image.
    // wait for results from the default capture queue.
    int requestNr = fi.imageRequestWaitFor( iMaxWaitTime_ms );
    manuallyStopAcquisitionIfNeeded( pDev, fi );
    // check if the image has been captured without any problems.
    if( !fi.isRequestNrValid( requestNr ) )
    {
        // If the error code is -2119(DEV_WAIT_FOR_REQUEST_FAILED), the documentation will provide
        // additional information under TDMR_ERROR in the interface reference
        cout << "imageRequestWaitFor failed (" << requestNr << ", " << ImpactAcquireException::getErrorCodeAsString( requestNr ) << ")"
             << ", timeout value too small?" << endl;
        return 0;
    }

    const Request* pRequest = fi.getRequest( requestNr );
    if( !pRequest->isOK() )
    {
        cout << "Error: " << pRequest->requestResult.readS() << endl;
        // if the application wouldn't terminate at this point this buffer HAS TO be unlocked before
        // it can be used again as currently it is under control of the user. However terminating the application
        // will free the resources anyway thus the call
        // fi.imageRequestUnlock( requestNr );
        // can be omitted here.
        return 0;
    }

    cout << "Please note that there will be just one refresh for the display window, so if it is" << endl
         << "hidden under another window the result will not be visible." << endl;
    // everything went well. Display the result
    ImageDisplayWindow display( "mvIMPACT_acquire sample" );
    display.GetImageDisplay().SetImage( pRequest );
    display.GetImageDisplay().Update();

    cout << "Image captured( " << pRequest->imagePixelFormat.readS() << " " << pRequest->imageWidth.read() << "x" << pRequest->imageHeight.read() << " )" << endl;
    // unlock the buffer to let the driver know that you no longer need this buffer.
    fi.imageRequestUnlock( requestNr );

    cout << "Press [ENTER] to end the application" << endl;
    cin.get();

    return a.exec();
}
