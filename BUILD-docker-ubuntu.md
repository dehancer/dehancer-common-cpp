# Prerequisites
    # docker permissions
```bash
    sudo groupadd docker
    sudo usermod -aG docker ${USER}
    sudo chown "$USER":"$USER" /home/"$USER"/.docker -R
    sudo chmod g+rwx "$HOME/.docker" -R
    sudo chmod 666 /var/run/docker.sock
    docker run hello-world
```
    # build dockerfile
```bash
sudo docker build --progress=plain --build-arg UID=$(id -u) -t clion/ubuntu/cpp-env:1.0 -f Dockerfile.cpp-env-ubuntu .
```