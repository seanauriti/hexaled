<?php
    header('Content-type: text/xml');

    include "php_serial.class.php";
    include "config.php";
    $f = new phpSerial;
    $f->deviceSet($SERIAL_PORT);
    $f->confBaudRate(19200);
    $f->confFlowControl("none");
    $f->deviceOpen();

    # See if we got here from a form submission
    if ($_REQUEST["cmd"])
    {
        $f->sendMessage($_REQUEST["cmd"]);
    }
    $f->sendMessage("Q");
    $currentstate = $f->readPort();
?><status>
    <decimal><?= $currentstate ?></decimal>
</status>
