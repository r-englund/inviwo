node {
    stage('Fetch') { 
        dir('inviwo') {
            checkout scm
            sh 'git submodule sync --recursive' // needed when a submodule has a new url  
            sh 'git submodule update --init --recursive'
        }
    }
    println env.getEnvironment().inject('\nEnv:\n', { res, item -> res + "${item.key.padLeft(25)} = ${item.value}\n" })

    def util = load "${env.WORKSPACE}/inviwo/tools/jenkins/util.groovy"      
    properties(util.defaultProperties(env))

    List modulePaths = []
    List on = []
    List off = ["ABUFFERGL"]

    Map state = [
        env: env,
        build: currentBuild, 
        errors: [],
        display: 0,
        addLabel: {label -> 
            println("Add label: ${label}")
            if (env.CHANGE_ID  & (!label in pullRequest.labels)) {
                pullRequest.addLabels([label])
            }
        },
        removeLabel: {label -> 
            println("Remove label: ${label}")
            if (env.CHANGE_ID & label in pullRequest.labels) {
                pullRequest.removeLabel([label])
            }
        }
    ]

    try {
        util.buildStandard(
            state: state,
            modulePaths: modulePaths, 
            onModules: on,  
            offModules: off,
            opts: opts
        )
        util.filterfiles()
        util.format(state)
        util.warn(state)
        util.unittest(state)
        util.integrationtest(state)        
        util.regression(state, ["${env.WORKSPACE}/inviwo/modules"])
        util.copyright(state)    
        util.doxygen(state)

        state.build.result = state.errors.isEmpty() ? 'SUCCESS' : 'FAILURE'
    } catch (e) {
        state.build.result = 'FAILURE'
        throw e
    } finally {
        util.slack(state, "#jenkins-branch-pr")
    }
}
