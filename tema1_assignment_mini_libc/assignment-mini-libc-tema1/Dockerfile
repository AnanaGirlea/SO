FROM gitlab.cs.pub.ro:5050/operating-systems/assignments-docker-base

RUN apt install -yqq nasm
RUN apt install -yqq strace
RUN apt install -yqq ltrace
RUN apt install -yqq binutils

COPY ./checker ${CHECKER_DATA_DIRECTORY}
RUN mkdir ${CHECKER_DATA_DIRECTORY}/../tests
COPY ./tests ${CHECKER_DATA_DIRECTORY}/../tests
