<?php
    header('Content-type: text/xml');

    include "config.php";
    $f = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
    $rc = socket_connect($f, "127.0.0.1", 11111);
    if ($rc === false) {
        echo "socket_connect() failed.\nReason: ($rc) " . socket_strerror(socket_last_error($f)) . "\n";
        exit(1);
    }

    # See if we got here from a form submission
    if (strlen($_REQUEST["cmd"]) > 0)
    {
        socket_write($f, $_REQUEST["cmd"], strlen($_REQUEST["cmd"]));
    }
    socket_write($f, "Q", 1);
    $currentstate = socket_read($f, 80);
    socket_close($f);
    
?><?= $currentstate ?>
