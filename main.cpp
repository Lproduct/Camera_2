#include <QCoreApplication>
#include <iostream>
#include <Common/exampleHelper.h>
#include <mvIMPACT_CPP/mvIMPACT_acquire.h>
#include <mvDisplay/Include/mvIMPACT_acquire_display.h>

using namespace std;
using namespace mvIMPACT::acquire;
using namespace mvIMPACT::acquire::display;

static bool g_boTerminated = false;

//-----------------------------------------------------------------------------
struct ThreadParameter
//-----------------------------------------------------------------------------
{
    Device*             pDev;
    ImageDisplayWindow  displayWindow;
    ThreadParameter( Device* p, const string& windowTitle ) : pDev( p ), displayWindow( windowTitle ) {}
};

//-----------------------------------------------------------------------------
unsigned int __stdcall liveThread( void* pData )
//-----------------------------------------------------------------------------
{
    ThreadParameter* pThreadParameter = reinterpret_cast<ThreadParameter*>( pData );
    unsigned int cnt = 0;

    cout << "Initialising the device. This might take some time..." << endl;
    try
    {
        pThreadParameter->pDev->open();
    }
    catch( const ImpactAcquireException& e )
    {
        // this e.g. might happen if the same device is already opened in another process...
        cout << "An error occurred while opening the device " << pThreadParameter->pDev->serial.read()
             << "(error code: " << e.getErrorCodeAsString() << ")." << endl
             << "Press [ENTER] to end the application..." << endl;
        cin.get();
        return 0;
    }

    ImageDisplay& display = pThreadParameter->displayWindow.GetImageDisplay();
    // establish access to the statistic properties
    Statistics statistics( pThreadParameter->pDev );
    // create an interface to the device found
    mvIMPACT::acquire::FunctionInterface fi( pThreadParameter->pDev );

    // Send all requests to the capture queue. There can be more than 1 queue for some device, but for this sample
    // we will work with the default capture queue. If a device supports more than one capture or result
    // queue, this will be stated in the manual. If nothing is set about it, the device supports one
    // queue only. This loop will send all requests currently available to the driver. To modify the number of requests
    // use the property mvIMPACT::acquire::SystemSettings::requestCount at runtime or the property
    // mvIMPACT::acquire::Device::defaultRequestCount BEFORE opening the device.
    TDMR_ERROR result = DMR_NO_ERROR;
    while( ( result = static_cast<TDMR_ERROR>( fi.imageRequestSingle() ) ) == DMR_NO_ERROR ) {};
    if( result != DEV_NO_FREE_REQUEST_AVAILABLE )
    {
        cout << "'FunctionInterface.imageRequestSingle' returned with an unexpected result: " << result
             << "(" << mvIMPACT::acquire::ImpactAcquireException::getErrorCodeAsString( result ) << ")" << endl;
    }

    manuallyStartAcquisitionIfNeeded( pThreadParameter->pDev, fi );
    // run thread loop
    mvIMPACT::acquire::Request* pRequest = 0;
    // we always have to keep at least 2 images as the displayWindow module might want to repaint the image, thus we
    // can free it unless we have a assigned the displayWindow to a new buffer.
    mvIMPACT::acquire::Request* pPreviousRequest = 0;
    const unsigned int timeout_ms = 500;
    while( !g_boTerminated )
    {
        // wait for results from the default capture queue
        int requestNr = fi.imageRequestWaitFor( timeout_ms );
        pRequest = fi.isRequestNrValid( requestNr ) ? fi.getRequest( requestNr ) : 0;
        if( pRequest )
        {
            if( pRequest->isOK() )
            {
                ++cnt;
                // here we can displayWindow some statistical information every 100th image
                if( cnt % 100 == 0 )
                {
                    cout << "Info from " << pThreadParameter->pDev->serial.read()
                         << ": " << statistics.framesPerSecond.name() << ": " << statistics.framesPerSecond.readS()
                         << ", " << statistics.errorCount.name() << ": " << statistics.errorCount.readS()
                         << ", " << statistics.captureTime_s.name() << ": " << statistics.captureTime_s.readS() << endl;
                }
                display.SetImage( pRequest );
                display.Update();
            }
            else
            {
                cout << "Error: " << pRequest->requestResult.readS() << endl;
            }
            if( pPreviousRequest )
            {
                // this image has been displayed thus the buffer is no longer needed...
                pPreviousRequest->unlock();
            }
            pPreviousRequest = pRequest;
            // send a new image request into the capture queue
            fi.imageRequestSingle();
        }
        else
        {
            // If the error code is -2119(DEV_WAIT_FOR_REQUEST_FAILED), the documentation will provide
            // additional information under TDMR_ERROR in the interface reference
            cout << "imageRequestWaitFor failed (" << requestNr << ", " << ImpactAcquireException::getErrorCodeAsString( requestNr ) << ")"
                 << ", timeout value too small?" << endl;
        }
    }
    manuallyStopAcquisitionIfNeeded( pThreadParameter->pDev, fi );

    // stop the displayWindow from showing freed memory
    display.SetImage( reinterpret_cast<Request*>( 0 ) );
    // In this sample all the next lines are redundant as the device driver will be
    // closed now, but in a real world application a thread like this might be started
    // several times an then it becomes crucial to clean up correctly.

    // free the last potential locked request
    if( pRequest )
    {
        pRequest->unlock();
    }
    // clear the request queue
    fi.imageRequestReset( 0, 0 );
    // extract and unlock all requests that are now returned as 'aborted'
    int requestNr = INVALID_ID;
    while( ( requestNr = fi.imageRequestWaitFor( 0 ) ) >= 0 )
    {
        pRequest = fi.getRequest( requestNr );
        cout << "Request " << requestNr << " did return with status " << pRequest->requestResult.readS() << endl;
        pRequest->unlock();
    }
    return 0;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    DeviceManager devMgr;
    Device* pDev = getDeviceFromUserInput( devMgr );
    if( !pDev )
    {
        cout << "Unable to continue!";
        cout << "Press [ENTER] to end the application" << endl;
        cin.get();
        return 0;
    }

    // start the execution of the 'live' thread.
    cout << "Press [ENTER] to end the application" << endl;
    unsigned int dwThreadID;
    string windowTitle( "mvIMPACT_acquire sample, Device " + pDev->serial.read() );
    // initialise displayWindow window
    // IMPORTANT: It's NOT save to create multiple displayWindow windows in multiple threads!!!
    ThreadParameter threadParam( pDev, windowTitle );
    HANDLE hThread = ( HANDLE )_beginthreadex( 0, 0, liveThread, ( LPVOID )( &threadParam ), 0, &dwThreadID );
    cin.get();
    g_boTerminated = true;
    WaitForSingleObject( hThread, INFINITE );
    CloseHandle( hThread );

    return a.exec();
}
