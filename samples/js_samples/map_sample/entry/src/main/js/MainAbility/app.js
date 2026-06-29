let Intervalid = -1;
let Timeoutid = -1;

function setAppGlobalTimer() {
    let intervalid = setAppGlobalInterval(function () {

        console.info("app setAppGlobalInterval global")
    }, 5000);

    if (intervalid) {

        console.info("setAppGlobalInterval success")
        Intervalid = intervalid
    } else {
        console.error("setAppGlobalInterval error")
    }

    let timeoutid = setAppGlobalTimeout(function () {
        console.info("app setAppGlobalTimeout global")
    }, 10000);

    if (timeoutid) {
        console.info("setAppGlobalTimeout success")
        Timeoutid = timeoutid
    } else {
        console.error("setAppGlobalTimeout error")
    }
}

function deleteAppGlobalTimer() {
    console.info("deleteAppGlobalTimer")
    clearInterval(Intervalid);
    clearTimeout(Timeoutid);
}

export default {
    data:{
        setAppGlobalTimer:setAppGlobalTimer,
        deleteAppGlobalTimer:deleteAppGlobalTimer
    },
    onCreate() {
        console.info('Application onCreate');
    },
    onDestroy() {
        console.info('Application onDestroy');
    }
};
