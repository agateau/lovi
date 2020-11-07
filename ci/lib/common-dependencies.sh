install_qpropgen_dependencies() {
    echo_title "Installing qpropgen dependencies"
    python -m pip install -r $SRC_DIR/third-party/qpropgen/requirements.txt
}
